package nl.klpd.tde.ocfamodule.ole2txt;

import java.io.IOException;
import java.io.InputStream;
import java.util.Map.Entry;

import org.apache.poi.poifs.filesystem.POIFSFileSystem;

import nl.klpd.tde.ocfa.evidence.Evidence;
import nl.klpd.tde.ocfa.message.ModuleInstance;
import nl.klpd.tde.ocfa.misc.OcfaException;
import nl.klpd.tde.ocfa.module.OcfaModule;
import nl.klpd.tde.ocfa.store.EvidenceStoreEntity;

public class Ole2TxtModule extends OcfaModule {

	private OcfaOleNodeProcessor processor = null;
	private OleDocWalker<Evidence> oleWalker = new OleDocWalker<Evidence>();
	public Ole2TxtModule(String inName, String inNameSpace)
			throws OcfaException {
		super(inName, inNameSpace);
		processor = new OcfaOleNodeProcessor();
		processor.setMailSender(this.getConnector().getMailSender());
		processor.setFactory(this.getFactory());
		processor.setRepository(getRepository());
		
		ModuleInstance router = new ModuleInstance();

		router.setNamespace("core");
		router.setModuleName("router");
		processor.setRouterAddress(router);
		oleWalker.setNodeProcessor(processor);
	}

	public Ole2TxtModule() throws OcfaException{
		this("ole2txt", "default");
	}
	
	@Override
	protected void processEvidence(Evidence evidence) {
		
		if (evidence.getDataHandle() != null){
		
			
			InputStream stream;
			try {
		//		System.out.println("Processing: " + evidence.getEvidencePath() + evidence.getEvidenceName());
				stream = getRepository().getDataEntityAsStream(evidence.getDataHandle());

				POIFSFileSystem system = new POIFSFileSystem(stream);
				processor.setFileSystem(system);
				oleWalker.processNode(system.getRoot(), evidence);
				for (Entry<String, String> entry :processor.getMetaMap().entrySet()){
					
					evidence.getActiveJob().setMeta(entry.getKey(), entry.getValue());
				}
				processor.resetMeta();
				
			} catch (OcfaException e) {

				e.printStackTrace();
				try {
					evidence.getActiveJob().setMeta("error", " ocfaexception: " + e.getMessage());
				} catch (OcfaException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			} catch (IOException e) {

				e.printStackTrace();
				try {
					evidence.getActiveJob().setMeta("error", "IOException: " + e.getMessage());
				} catch (OcfaException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}

			} catch (Exception e) {

				e.printStackTrace();
				try {
					evidence.getActiveJob().setMeta("error", "Exception (" + e.getClass().toString() + "): " + e.getMessage());
				} catch (OcfaException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}

			}
			
		}
			
	}

	public static void main(String argv[]){
		
		Ole2TxtModule module;
		try {
			module = new Ole2TxtModule("ole2txt", "default");
			module.run();
			

		} catch (OcfaException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
}
