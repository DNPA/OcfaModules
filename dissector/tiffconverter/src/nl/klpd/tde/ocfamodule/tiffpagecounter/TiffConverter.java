package nl.klpd.tde.ocfamodule.tiffpagecounter;

import java.awt.image.RenderedImage;
import java.awt.image.renderable.ParameterBlock;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import javax.media.jai.JAI;
import javax.media.jai.RenderedOp;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import nl.klpd.tde.ocfa.evidence.Evidence;
import nl.klpd.tde.ocfa.evidence.ValueType;
import nl.klpd.tde.ocfa.misc.OcfaException;
import nl.klpd.tde.ocfa.module.OcfaDissector;
import nl.klpd.tde.ocfa.store.EvidenceStoreEntity;

import com.sun.media.jai.codec.ImageCodec;
import com.sun.media.jai.codec.ImageDecoder;
import com.sun.media.jai.codec.TIFFEncodeParam;

public class TiffConverter extends OcfaDissector {

	Log log  = LogFactory.getLog(this.getClass());
	public TiffConverter() throws OcfaException {
		super("tiffconverter", "default");
	}

	@Override
	protected void processEvidence(Evidence inEvidence) {
		// TODO Auto-generated method stub
		try {
			InputStream ss = getRepository().getDataEntityAsStream(
					inEvidence.getDataHandle());
			ImageDecoder dec = ImageCodec.createImageDecoder("tiff", ss, null);
			int count;
			try {
				count = dec.getNumPages();
				log.info(inEvidence.getEvidenceName() + " has " + count + " pages");
				inEvidence.getActiveJob().setMeta("pagecount",
						Integer.toString(count), ValueType.INT);

				if (count > 1){
				TIFFEncodeParam param = new TIFFEncodeParam();
				param.setCompression(TIFFEncodeParam.COMPRESSION_GROUP4);
				param.setLittleEndian(false);
				for (int i = 0; i < count; i++) {

					log.info("extracting page" + i);
					RenderedImage page = dec.decodeAsRenderedImage(i);
					File f = new File(this.getWorkDir(), "page-" + i);
					ParameterBlock pb = new ParameterBlock();
					pb.addSource(page);
					pb.add(f.toString());
					pb.add("tiff");
					pb.add(param);
					RenderedOp r = JAI.create("filestore", pb);
					r.dispose();
					deriveNewFileEntity(inEvidence, f);
					log.info(f.getName() + " extracted");

				}
				}

			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} catch (OcfaException e) {

			e.printStackTrace();

		}
	}


	public void deriveNewFileEntity(Evidence parentEvidence, File inFile)
			throws OcfaException {
		long fileSize = inFile.length();
		EvidenceStoreEntity entity = getRepository().createEvidenceStoreEntity(
				inFile);
		Evidence newEvidence = this.getFactory().createEvidence(
				entity.getHandle(), entity.getPair(), inFile.getName(),
				parentEvidence, "output");
		newEvidence.getActiveJob().setMeta("size", Long.toString(fileSize),
				ValueType.INT);
		newEvidence.getActiveJob().setMeta("mimetype", "image/tiff");
		newEvidence.getActiveJob().setMeta("mimetop", "image");
		newEvidence.getActiveJob().setMeta("nodetype", "file");
		newEvidence.getActiveJob().setMeta("pagecount", "1", ValueType.INT);

		sendNewMessage(newEvidence);

	}
}
