package nl.klpd.tde.ocfamodule.ole2txt;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.SequenceInputStream;

import org.apache.poi.POITextExtractor;
import org.apache.poi.extractor.ExtractorFactory;
import org.apache.poi.hpsf.MarkUnsupportedException;
import org.apache.poi.hpsf.NoPropertySetStreamException;
import org.apache.poi.hpsf.PropertySetFactory;
import org.apache.poi.hpsf.SummaryInformation;
import org.apache.poi.poifs.filesystem.DirectoryNode;
import org.apache.poi.poifs.filesystem.Entry;
import org.apache.poi.poifs.filesystem.POIFSFileSystem;

import nl.klpd.tde.ocfa.evidence.Evidence;
import nl.klpd.tde.ocfa.evidence.EvidenceFactory;
import nl.klpd.tde.ocfa.message.CastType;
import nl.klpd.tde.ocfa.message.ContentType;
import nl.klpd.tde.ocfa.message.Message;
import nl.klpd.tde.ocfa.message.MessageType;
import nl.klpd.tde.ocfa.message.ModuleInstance;
import nl.klpd.tde.ocfa.message.OutgoingMailBox;
import nl.klpd.tde.ocfa.misc.EvidenceIdentifier;
import nl.klpd.tde.ocfa.misc.OcfaException;
import nl.klpd.tde.ocfa.store.EvidenceStoreEntity;
import nl.klpd.tde.ocfa.store.Repository;
import nl.klpd.tde.treegraphwalker.NodeProcessor;

public class OcfaOleNodeProcessor implements
		NodeProcessor<Entry, Evidence, Exception> {

	private EvidenceFactory factory;
	private POIFSFileSystem fileSystem;
	private OutgoingMailBox mailSender;
	private ModuleInstance routerAddress;
	public OutgoingMailBox getMailSender() {
		return mailSender;
	}

	public void setMailSender(OutgoingMailBox mailSender) {
		this.mailSender = mailSender;
	}

	public ModuleInstance getRouterAddress() {
		return routerAddress;
	}

	public void setRouterAddress(ModuleInstance routerAddress) {
		this.routerAddress = routerAddress;
	}


	private Repository repository;
	
	public Repository getRepository() {
		return repository;
	}

	public void setRepository(Repository repository) {
		this.repository = repository;
	}

	public POIFSFileSystem getFileSystem() {
		return fileSystem;
	}

	public void setFileSystem(POIFSFileSystem fileSystem) {
		this.fileSystem = fileSystem;
	}

	public EvidenceFactory getFactory() {
		return factory;
	}

	public void setFactory(EvidenceFactory factory) {
		this.factory = factory;
	}

	public Evidence processAfter(Entry inNode, Evidence ownState)
			throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	public Evidence processBefore(Entry inNode, Evidence inParentState)
			throws Exception {
	
		
		if (inNode instanceof DirectoryNode){
			
			return inParentState;
			
		} else {
			
			String name = inNode.getName();
			if (name.equalsIgnoreCase("Workbook")){
				
				
				sendEvidenceFromExcel(inNode, inParentState);
			
			} else if (name.equalsIgnoreCase("\005SummaryInformation")){
				
				addSummaryInformation(inParentState, inNode);
			} else if (name.equalsIgnoreCase("WordDocument")){
				
				sendEvidenceFromWord(inNode, inParentState);
			}
		}
		return null;
	}

	private void sendEvidenceFromWord(Entry inNode, Evidence inEvidence) throws IOException, OcfaException{
		
		POITextExtractor extractor = ExtractorFactory.createExtractor((DirectoryNode) inNode.getParent(), 
				fileSystem);
		EvidenceStoreEntity storeEntity = 
			repository.createEvidenceStoreEntity(new ByteArrayInputStream(extractor.getText().getBytes()));
		Evidence evidence = factory.createEvidence(storeEntity.getHandle(), storeEntity.getPair(), "output", 
				inEvidence, "word");
		evidence.getActiveJob().setMeta("mimetop", "text");
		evidence.getActiveJob().setMeta("mimetype", "plain/text");
		evidence.getActiveJob().setMeta("encoding", "utf-8");
		evidence.getActiveJob().setMeta("inodetype", "file");
		evidence.getActiveJob().setMeta("nodetype", "file");

		evidence.getActiveJob().setMeta("size" , String.valueOf(extractor.getText().getBytes().length));
		sendNewEvidence(evidence);
		
	}

	private void addSummaryInformation(Evidence inEvidence, Entry inNode) throws IOException, 
		NoPropertySetStreamException, MarkUnsupportedException {
		
		InputStream stream = ((DirectoryNode)inNode.getParent()).createDocumentInputStream(inNode.getName());
		SummaryInformation si = (SummaryInformation) PropertySetFactory.create(stream);
		
		addOptionalMeta(inEvidence, "author", si.getAuthor());
		
		addOptionalMeta(inEvidence,"applicationName", si.getApplicationName());
		addOptionalMeta(inEvidence,"title", si.getTitle());

		
	}

	private void addOptionalMeta(Evidence inEvidence, String metaName, String inValue) {
		
		if (inValue != null){
			
			try {
				inEvidence.getActiveJob().setMeta(metaName, inValue);
			} catch (OcfaException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
	}

	private void sendEvidenceFromExcel(Entry inNode,
			Evidence inParentEvidence) throws IOException, OcfaException {
		
		Excel2Txt excel2txt = 
			new Excel2Txt(fileSystem, (DirectoryNode)inNode.getParent());
		for (int x= 0; x < excel2txt.getAmountOfSubFiles(); x++){
		
			EvidenceStoreEntity storeEntity = 
				repository.createEvidenceStoreEntity(new ByteArrayInputStream(excel2txt.getSubPageContent(x).getBytes()));
			Evidence evidence = factory.createEvidence(storeEntity.getHandle(), storeEntity.getPair(), excel2txt.getSubPageName(x), 
					inParentEvidence, "sheet");
			evidence.getActiveJob().setMeta("size",
					String.valueOf(excel2txt.getSubPageContent(x).getBytes().length));
			evidence.getActiveJob().setMeta("mimetop", "text");
			evidence.getActiveJob().setMeta("mimetype", "plain/text");
			evidence.getActiveJob().setMeta("encoding", "utf-8");
			evidence.getActiveJob().setMeta("inodetype", "file");
			
			sendNewEvidence(evidence);			
		}
		
	}

		
	public void sendNewEvidence(Evidence inEvidence) throws OcfaException{
		inEvidence.getActiveJob().close();
		String evidenceString = factory.evidenceAsString(inEvidence);
		EvidenceIdentifier identifier= new EvidenceIdentifier();
		identifier.setCaseName(inEvidence.getCase());
		identifier.setEvidenceSourceID(inEvidence.getSource());
		identifier.setItemName(inEvidence.getItem());
		identifier.setEvidenceID(inEvidence.getEvidenceId());
		String metaHandle = repository.insertMeta(evidenceString, inEvidence.getDataHandle(), identifier);
		
		Message message = mailSender.getMessageFactory().createMessage();
		message.setContent(metaHandle);
		message.setCastType(CastType.ANYCAST);
		message.setReceiver(routerAddress);
		message.setPrio(6);
		message.setContentType(ContentType.EVIDENCE);
		message.setMessageType(MessageType.USER);
		message.setSubject("newevidence");
		mailSender.sendMessage(message);
		
	}

}
