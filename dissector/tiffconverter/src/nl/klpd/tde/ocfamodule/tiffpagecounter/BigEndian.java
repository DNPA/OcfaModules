package nl.klpd.tde.ocfamodule.tiffpagecounter;

import java.io.IOException;
import java.io.InputStream;

public class BigEndian implements ByteOrder {

	// not complete. it assumes int below 2 Gb.
	public long readInt(InputStream stream) throws IOException {

		int firstByte;
		int secondByte;
		int thirdByte;
		int fourthByte;
		byte buffer[] =  new byte[4];
		int bytesRead = stream.read(buffer);
		if (bytesRead != 4){;
		
			throw new IOException("wanted to read: 4. Read " + bytesRead);
		}
		firstByte = (0x00000000FF) & ((int)buffer[0]);
		secondByte = (0x00000000FF) & ((int)buffer[1]);
		thirdByte = (0x00000000FF) & ((int)buffer[2]);
		fourthByte = (0x00000000FF) & ((int)buffer[3]);
		
		long anUnsignedInt = ((long)(firstByte << 24
				| secondByte << 16 
				| thirdByte << 8
				| fourthByte)) & 0xFFFFFFFFL;
		return anUnsignedInt;
	}

	public char readShort(InputStream stream) throws IOException {
		int firstByte;
		int secondByte;
		byte buffer[] =  new byte[2];
		int bytesRead = stream.read(buffer);
		if (bytesRead != 2){;
		
			throw new IOException("wanted to read: 4. Read " + bytesRead);
		}

		firstByte = (0x00000000FF) & ((int)buffer[0]);
		secondByte = (0x00000000FF) & ((int)buffer[1]);
		char anUnsignedShort = (char)(firstByte << 8 | secondByte);
		return anUnsignedShort;
	}

	
	public OrderName getName() {
		// TODO Auto-generated method stub
		return ByteOrder.OrderName.BigEndian;
	}

}
