package nl.klpd.tde.ocfamodule.ole2txt.gui;

import java.io.IOException;

import javax.swing.JTextArea;

import org.apache.poi.hpsf.MarkUnsupportedException;
import org.apache.poi.hpsf.NoPropertySetStreamException;
import org.apache.poi.hpsf.PropertySetFactory;
import org.apache.poi.hpsf.SummaryInformation;
import org.apache.poi.hssf.OldExcelFormatException;
import org.apache.poi.poifs.filesystem.DirectoryEntry;
import org.apache.poi.poifs.filesystem.DirectoryNode;
import org.apache.poi.poifs.filesystem.DocumentInputStream;
import org.apache.poi.poifs.filesystem.DocumentNode;
import org.apache.poi.poifs.filesystem.Entry;
import org.apache.poi.poifs.filesystem.EntryNode;
import org.apache.poi.poifs.filesystem.POIFSFileSystem;

import nl.klpd.tde.ocfamodule.ole2txt.Excel2Txt;
import nl.klpd.tde.treegraphwalker.NodeProcessor;

public class SimpleOleNodeProcessor implements NodeProcessor<Entry, String,Exception> {

	private POIFSFileSystem fileSystem;
	private JTextArea outputArea;
	public POIFSFileSystem getFileSystem() {
		return fileSystem;
	}

	public void setFileSystem(POIFSFileSystem fileSystem) {
		this.fileSystem = fileSystem;
	}

	public String processAfter(Entry inNode, String ownState) throws Exception {
		return " ";
	}

	public String processBefore(Entry inNode, String inParentState)
			throws Exception {
		
		if (inNode instanceof DirectoryNode){
			
			return inParentState + " ";
		} else {
			
			String name = inNode.getName();
			if (name.equalsIgnoreCase("Workbook") || name.equalsIgnoreCase("Book")){
				
				try {
				Excel2Txt excel2txt = 
					new Excel2Txt(fileSystem, (DirectoryNode)inNode.getParent());
				printExcelToText(excel2txt, inParentState);
				} catch (OldExcelFormatException e){
					
					
					
				}
			} else if (name.equalsIgnoreCase("\005SummaryInformation")){
				
				printSummaryInformation(inParentState, inNode);
			}
		}
		return "";
	}

	private void printSummaryInformation(String inParentState, Entry inNode) {

		DocumentInputStream stream;
		try {
			stream = ((DirectoryNode)inNode.getParent()).createDocumentInputStream(inNode.getName());
			SummaryInformation si = (SummaryInformation) PropertySetFactory.create(stream);
			outputArea.append(inParentState + "Author: " + si.getAuthor() + "\n");
			outputArea.append(inParentState + "ApplicationName " + si.getApplicationName() + "\n");
			outputArea.append(inParentState + "Title: "  + si.getTitle()+ "\n");
		
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoPropertySetStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (MarkUnsupportedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}

	public JTextArea getOutputArea() {
		return outputArea;
	}

	public void setOutputArea(JTextArea outputArea) {
		this.outputArea = outputArea;
	}

	private void printExcelToText(Excel2Txt excel2txt, String inParentState) {
	
		
		for (int x =0; x < excel2txt.getAmountOfSubFiles(); x++){
			
			outputArea.append("Sheet " + x + "\n");
			outputArea.append(excel2txt.getSubPageContent(x));
		}
		
		
	}




}
