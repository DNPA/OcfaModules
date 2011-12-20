Photorec is a module based on the photorec program from Christophe GRENIER.
This module will extract hidden files from unallocated clusters.

TestDisk & PhotoRec , http://www.cgsecurity.org
Copyright (C) 1998-2008 Christophe GRENIER <grenier@cgsecurity.org>


Rulelist
========
You need an extra rulelist rule to send unallocated parts to the module.
Example of rulelist rule:
  default;DNTCR;fileextension;unaloc;DNTCR;ACT_FORWARD;photorec;



Script
======
The photorec module uses a bash script to invoke the photorec programm.
In this "ocfacarv.sh" shell script, the user can add or delete file types
to extract from unallocated space, by adding comma separated values of
file extensions, like:
	7z,enable,
	a,enable,
	etc.

The "ocfacrv.sh" script will be installed in de ${OCFAROOT}/sbin directory.
By editing this file, the user can make case specific behaviour changes.


Complete list of files to carve:
================================
7z,enable,a,enable,accdb,enable,ace,enable
,ab,enable,aif,enable,all,enable,als,enable,amd,enable,amr,enable,arj,enable,asf,enable,asm,enable,au,enable,bkf,enable,blend,enabl
e,bmp,enable,bz2,enable,cab,enable,cam,enable,chm,enable,comicdoc,enable,crw,enable,ctg,enable,cwk,enable,dat,enable,dbf,disable,di
skimage,enable,djv,enable,drw,enable,doc,enable,dpx,enable,ds2,enable,dsc,enable,dss,enable,dta,disable,dump,enable,dv,enable,dwg,e
nable,elf,enable,emf,enable,evt,enable,exe,enable,fcp,enable,fcs,enable,fh10,enable,fh5,enable,fits,enable,flac,enable,flv,enable,f
rm,enable,fs,enable,gho,enable,gif,enable,gpg,enable,gz,enable,ifo,enable,imb,enable,indd,enable,iso,enable,itu,enable,jpg,enable,k
db,enable,lnk,enable,m2ts,enable,max,enable,mb,enable,mcd,enable,mdb,enable,mdf,enable,mfg,enable,mid,enable,mkv,enable,mov,enable,
mp3,enable,mpg,enable,mrw,enable,mus,enable,MYI,enable,njx,enable,ogg,enable,one,enable,orf,enable,paf,enable,pap,enable,pct,enable
,pcx,enable,pdf,enable,pfx,enable,png,enable,prc,enable,prt,enable,ps,enable,psd,enable,pst,enable,ptb,enable,qbb,enable,qdf,enable
,qxd,enable,ra,enable,raf,enable,rar,enable,raw,enable,rdc,enable,reg,enable,res,enable,riff,enable,rm,enable,rns,enable,rpm,enable
,SIT,enable,skp,enable,spe,enable,sav,enable,sqlite,enable,stl,enable,sit,enable,swf,enable,tar,enable,tib,enable,tif,enable,tph,en
able,tx?,enable,txt,enable,vmdk,enable,wpd,enable,wmf,enable,x3f,enable,xcf,enable,xm,enable,xsv,enable,veg,enable,wks,enable,zip,e
nable



