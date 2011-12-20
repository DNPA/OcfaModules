package nl.klpd.tde.ocfamodule.tiffpagecounter;

import java.io.IOException;
import java.io.InputStream;

public interface ByteOrder {

	public enum OrderName {
		
		LittleEndian,BigEndian
	}
	long readInt(InputStream stream) throws IOException;
	char readShort(InputStream stream) throws IOException;
	OrderName getName();
}
