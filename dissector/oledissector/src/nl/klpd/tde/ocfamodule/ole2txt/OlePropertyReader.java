package nl.klpd.tde.ocfamodule.ole2txt;

import java.io.IOException;
import java.io.InputStream;
import java.util.Date;

import org.apache.poi.hpsf.PropertySetFactory;
import org.apache.poi.hpsf.SummaryInformation;
import org.apache.poi.poifs.eventfilesystem.POIFSReader;
import org.apache.poi.poifs.eventfilesystem.POIFSReaderEvent;
import org.apache.poi.poifs.eventfilesystem.POIFSReaderListener;

public class OlePropertyReader {

	
	private SummaryInformation si;
	public OlePropertyReader(InputStream inStream) throws IOException{
		
		POIFSReader reader = new POIFSReader();
		reader.registerListener(new OleEventProcessor(), "\005SummaryInformation");
		reader.read(inStream);
	}
	
	
	
	protected class OleEventProcessor implements POIFSReaderListener {

		public void processPOIFSReaderEvent(POIFSReaderEvent inEvent) {

			si = null;
			try {
				
				si = (SummaryInformation) PropertySetFactory.create(inEvent.getStream());
			} catch(Exception e){
				
				e.printStackTrace();
			}
			
		}
	
	}
	
	public String getTitle(){
		
		return si.getTitle();
	}

	public String getAuthor(){
		
		return si.getAuthor();		
	}
	
	public Date getCreateDateTime(){
	
		return si.getCreateDateTime();
	}
	
}
