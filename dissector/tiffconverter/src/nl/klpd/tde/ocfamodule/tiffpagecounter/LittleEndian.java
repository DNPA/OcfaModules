package nl.klpd.tde.ocfamodule.tiffpagecounter;

import java.io.IOException;
import java.io.InputStream;

public class LittleEndian implements ByteOrder {

	public OrderName getName() {
		// TODO Auto-generated method stub
		return OrderName.LittleEndian;
	}

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

		long anUnsignedInt = ((long)(firstByte 
				| secondByte << 8 
				| thirdByte << 16
				| fourthByte<< 24)) & 0xFFFFFFFFL;

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
		char anUnsignedShort = (char)(secondByte << 8 | firstByte);
		return anUnsignedShort;
		
	}

}
