package nl.klpd.tde.ocfamodule.tiffpagecounter;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

public class TiffPageCounter {

	
	
	public int countPages(InputStream stream) throws IOException{
		
		long currentPos = 0;
		ByteOrder order = determineByteOrder(stream);
		currentPos +=2;
		System.out.println("Byteorder is " + order.getName());
		int version = readVersion(stream, order);
		currentPos +=2;
		System.out.println("version is:" + version);
		if (version != 42){
			
			throw new IOException("expected 42. found " + version);
		}
		
		int pageNr = 0;
		long nextOffset = order.readInt(stream);
		currentPos +=4;
		while (nextOffset != 0){
			
			pageNr ++;
			stream.skip(nextOffset-currentPos);
			currentPos = nextOffset;
			char tagCount = order.readShort(stream);
			currentPos +=2;
			stream.skip(tagCount * 12);
			currentPos += (tagCount * 12);
			nextOffset = order.readInt(stream);	
			currentPos +=4;
		}
		return pageNr;
		
	}
	private int readVersion(InputStream stream, ByteOrder order) throws IOException {
		
		return order.readShort(stream);
	}
	
	private ByteOrder determineByteOrder(InputStream stream) throws IOException {
		// TODO Auto-generated method stub
		byte word[] = new byte[2];
		int amountRead = stream.read(word);
		if (amountRead != 2){
			
			throw new IOException("couldn't read more than " + amountRead);
		}
		if (word[0] == 'I' && word[1] == 'I'){
			
			return new LittleEndian();
		} else if (word[0] == 'M' && word[1] == 'M'){
			
			return new BigEndian();
		} else {
			
			throw new IOException("Expected byte order. Found: " + word[0] + word[1]);
		}
	}
	
	public static void main(String argv[]){
		
		try {
			TiffPageCounter counter = new TiffPageCounter();
			FileInputStream stream = new FileInputStream(argv[0]);
			System.out.println("Pagecount is : " + counter.countPages(stream));
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
	}
}
