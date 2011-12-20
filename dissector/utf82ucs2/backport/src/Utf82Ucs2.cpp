
//  The Open Computer Forensics Architecture moduleset.
//  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <error.h>        
#include <ocfa.hpp>
#include <Utf82Ucs2.hpp>
#include <stdexcept>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
using namespace ocfa;
using namespace evidence;

static int convert (iconv_t cd, FILE *outFile, const char *inFilename);
static int process_fd (iconv_t cd, int fd, FILE *output);
static int process_block (iconv_t cd, char *addr, size_t len, FILE *output);

//Utf82Ucs2::Utf82Ucs2()

int Utf82Ucs2::processEvidence() {

  iconv_t converter = iconv_open("ucs-2le", "utf-8");
  if (converter < 0){

    cerr << " No converter: " << errno;
    return 0;
  }
  //  char *line = 0;
  char *outputBuffer = (char *)malloc(1024 * sizeof(char));

  ocfa::store::EvidenceStoreEntity * evidence = createEvidenceStoreObject();
  string wd=getWorkDir();  
  if (evidence == 0){

    throw OcfaException("Evidence without evidence store",this);
  }
  
  string outputFile = wd + "/output";
  
  string inputFilePath = evidence->getAsFilePath();
  delete evidence;
  

  FILE *output = fopen(outputFile.c_str(), "w");
  cerr << " converting " << inputFilePath << " into " << outputFile << endl;
  if (convert(converter, output, inputFilePath.c_str()) != 0){

    cerr << "conversion error " << errno  << endl;
    switch(errno){
      
    case E2BIG:
       cerr << "E2BIG" << endl;
      break;
    case EILSEQ:
      cerr << "EILSEQ" << endl;
      break;
    case EINVAL:
      cerr << "EINVAL" << endl;
      break;
    }
    
  }

 
  fclose(output);


  DerivedEvidence *derived=derive("output",Scalar("output"));
  derived->pushBackMeta("encoding",Scalar("ucs-2le"));
  derived->pushBackMeta("mimetype", Scalar("text/plain"));
  derived->submit();
  delete derived;
  free(outputBuffer);
  iconv_close(converter);
  return 1;
}

void Utf82Ucs2::processMessage(msg::Message & imsg){
  
}
int main(int argc, char *argv[]){
  Utf82Ucs2 *module=0;
  try {
    module= new Utf82Ucs2();
  } catch(OcfaException ex) {

    cerr <<  "Ocfa Exeption cougth in module construction: " << ex.what() << "\n";
    return 2;
  }
  try {
     module->run();
  } catch(OcfaException ex) {
    module->syslog(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    module->syslog(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  module->PrintObjCount();
  delete module ;
  return 1;
}

static int convert (iconv_t cd, FILE *outFile, const char *inFileName){
  int fd, ret;
  
  
  fd = open (inFileName, O_RDONLY);
  
  if (fd == -1){
      
    throw OcfaException(string("Cannot open ") + inFileName);
  }
  
  
  
  /* Read the file in pieces.  */
  cerr << "Reading " << inFileName << "opening " << fd << endl;
  ret = process_fd (cd, fd, outFile);
  
  /* Now close the file.  */
  close (fd);

  return ret;

}





static int
process_fd (iconv_t cd, int fd, FILE *output)
{
  /* we have a problem with reading from a desriptor since we must not
     provide the iconv() function an incomplete character or shift
     sequence at the end of the buffer.  Since we have to deal with
     arbitrary encodings we must read the whole text in a buffer and
     process it in one step.  */
  static char *inbuf = NULL;
  static size_t maxlen = 0;
  size_t actlen = 0;
  char *inptr = inbuf + actlen;

  while (actlen < maxlen) {

      ssize_t n = read (fd, inptr, maxlen - actlen);

      if (n == 0)
	/* No more text to read.  */
	break;
      if (n == -1)
	{
	  /* Error while reading.  */
	  cerr << "breaking here" << endl;
	  cerr << "pointer is " << inptr << endl;
	  cerr << "maxlen is " << maxlen << endl;
	 
	  error(0, errno, "error while reading the input");
	  return -1;
	}
      
      inptr += n;
      actlen += n;
  }
  
  if (actlen == maxlen)
    while (1)
      {
	ssize_t n;
	char *new_inbuf;
	
	/* Increase the buffer.  */
	new_inbuf = (char *) realloc (inbuf, maxlen + 32768);
	if (new_inbuf == NULL)
	  {
	    cerr << "breaking here " << endl;
	    error (0, errno, "unable to allocate buffer for input");
	    return -1;
	  }
	inbuf = new_inbuf;
	maxlen += 32768;
	inptr = inbuf + actlen;

	do
	  {
	    n = read (fd, inptr, maxlen - actlen);
	    
	    if (n == 0)
	      /* No more text to read.  */
	      break;
	    
	    if (n == -1)
	      {
		/* Error while reading.  */
		cerr << "Breaking here. " << endl;
		error (0, errno, "error while reading the input");
		return -1;
	      }
	    
	    inptr += n;
	    actlen += n;
	  }
	while (actlen < maxlen);
	
	if (n == 0)
	  /* Break again so we leave both loops.  */
	  break;
      }
  
  /* Now we have all the input in the buffer.  Process it in one run.  */
  return process_block (cd, inbuf, actlen, output);
}



static int
process_block (iconv_t cd, char *addr, size_t len, FILE *output)
{
#define OUTBUF_SIZE	32768
  const char *start = addr;
  char outbuf[OUTBUF_SIZE];
  char *outptr;
  size_t outlen;
  size_t n;
  int ret = 0;

  while (len > 0)
    {
      outptr = outbuf;
      outlen = OUTBUF_SIZE;
      n = iconv (cd, &addr, &len, &outptr, &outlen);

      if (n == (size_t) -1 && errno == EILSEQ)
	{
	  ret = 1;
	  if (len == 0)
	    n = 0;
	  else
	    errno = E2BIG;
	}

      if (outptr != outbuf)
	{
	  /* We have something to write out.  */
	  int errno_save = errno;

	  if (fwrite (outbuf, 1, outptr - outbuf, output)
	      < (size_t) (outptr - outbuf)
	      || ferror (output))
	    {
	      /* Error occurred while printing the result.  */
	      error (0, 0, "\
conversion stopped due to problem in writing the output");
	      return -1;
	    }

	  errno = errno_save;
	}

      if (n != (size_t) -1)
	{
	  /* All the input test is processed.  For state-dependent
             character sets we have to flush the state now.  */
	  outptr = outbuf;
	  outlen = OUTBUF_SIZE;
	  n = iconv (cd, NULL, NULL, &outptr, &outlen);

	  if (outptr != outbuf)
	    {
	      /* We have something to write out.  */
	      int errno_save = errno;

	      if (fwrite (outbuf, 1, outptr - outbuf, output)
		  < (size_t) (outptr - outbuf)
		  || ferror (output))
		{
		  /* Error occurred while printing the result.  */
		  error (0, 0, "\
conversion stopped due to problem in writing the output");
		  return -1;
		}

	      errno = errno_save;
	    }

	  if (n != (size_t) -1)
	    break;

	  if (errno == EILSEQ)
	    {
	      ret = 1;
	      break;
	    }
	}

      if (errno != E2BIG)
	{
	  /* iconv() ran into a problem.  */
	  switch (errno)
	    {
	    case EILSEQ:
		error (0, 0, "illegal input sequence at position %ld",
		       (long int) (addr - start));
	      break;
	    case EINVAL:
	      error (0, 0, "\
incomplete character or shift sequence at end of buffer");
	      break;
	    case EBADF:
	      error (0, 0, "internal error (illegal descriptor)");
	      break;
	    default:
	      error (0, 0, "unknown iconv() error %d", errno);
	      break;
	    }

	  return -1;
	}
    }

  return ret;
}




 /*  ssize_t result; */
/*   while (getline(&line,&inputSize, input) != -1) { */

/*     char *tmpLine = line; */
/*     char *tmpOutput = outputBuffer; */
/*     if (inputSize > 1023){ */
      
/*       cerr << "Warning found a line longer than 1023 " << endl; */
/*       cerr << "only converting the first 1023 " << endl; */
/*     } */
/*     outputSize = 1023; */
/*     result = iconv(converter, &tmpLine, &inputSize, &tmpOutput, &outputSize); */
/*     if (result < 0){ */

/*       cerr << "Error: no result found " << result << endl; */
/*       cerr << "Error code " << errno << endl; */
/*     } */
/*     cerr << "line size is " << strlen(line) << endl; */
/*     free(line); */
/*     line = 0; */
/*     inputSize= 0; */
/*     fwrite(outputBuffer, outputBuffer - tmpOutput, sizeof(char), output); */
/*   } */
