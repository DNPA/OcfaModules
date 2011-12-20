package nl.klpd.tde.ocfamodule.ole2txt.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Iterator;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

import org.apache.poi.poifs.filesystem.DirectoryEntry;
import org.apache.poi.poifs.filesystem.DirectoryNode;
import org.apache.poi.poifs.filesystem.Entry;
import org.apache.poi.poifs.filesystem.POIFSFileSystem;

import nl.klpd.tde.ocfamodule.ole2txt.Excel2Txt;
import nl.klpd.tde.ocfamodule.ole2txt.OleDocWalker;
import nl.klpd.tde.ocfamodule.ole2txt.OlePropertyReader;

public class GuiPanel extends JPanel{

	private JLabel selectedFileLabel;
	private JTextArea resultArea;
	private JFileChooser choser = new JFileChooser();
	private SimpleDateFormat dateFormat = new SimpleDateFormat();
	GuiPanel(){
		
		setLayout(new BorderLayout());
		setSize(600, 800);
		setPreferredSize(new Dimension(600, 800));
		JPanel selectPanel = new JPanel();
		selectedFileLabel = new JLabel("no file selected");
		selectPanel.add(selectedFileLabel);
		JButton chooser = new JButton("choose file");
		chooser.addActionListener(new ChooseFileListener());
		selectPanel.add(chooser);
		add(selectPanel, BorderLayout.NORTH);
		
		resultArea = new JTextArea(20, 80);
		add(new JScrollPane(resultArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED), 
				BorderLayout.CENTER);
		//this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setVisible(true);
		setBackground(Color.BLUE);
		System.out.println("constructed");
	}
	
	
	protected class ChooseFileListener implements ActionListener {

		public void actionPerformed(ActionEvent e) {
			
			int returnValue = choser.showOpenDialog(GuiPanel.this);
			if (returnValue == JFileChooser.APPROVE_OPTION){
				
				File theFile = choser.getSelectedFile();
				selectedFileLabel.setText(theFile.getAbsolutePath());
				try {
					
					FileInputStream  stream= new FileInputStream(theFile);
					POIFSFileSystem fs = new POIFSFileSystem(stream);
					DirectoryNode root = fs.getRoot();
					SimpleOleNodeProcessor  nodeProcessor = new SimpleOleNodeProcessor();
					nodeProcessor.setFileSystem(fs);
					nodeProcessor.setOutputArea(resultArea);
					OleDocWalker<String> walker = new OleDocWalker<String>();
					//walker.setNodeProcessor(nodeProcessor);
					walker.setNodeProcessor(nodeProcessor);
					try {
						walker.processNode(root, "");
					} catch (Exception e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
				showContents(root, "");
//					stream.close();
//					stream = new FileInputStream(theFile);
//
//					OlePropertyReader reader = new OlePropertyReader(stream);
//					resultArea.append("Author:" + reader.getAuthor());
//					resultArea.append("\nTitle" + reader.getTitle());
//					resultArea.append("\nDate:" + dateFormat.format(reader.getCreateDateTime()));
//					resultArea.append("\n");
//					stream.close();
//					stream = new FileInputStream(theFile);
//					Excel2Txt converter = new Excel2Txt(stream);
//					for (int x = 0; x < converter.getAmountOfSubFiles(); x++){
//						
//						if (converter.getSubPageContent(x) != null){
//							resultArea.append(converter.getSubPageContent(x));
//					
//						}
//					}
					
					
				} catch (FileNotFoundException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				} catch (IOException ex) {
					// TODO Auto-generated catch block
					ex.printStackTrace();
				}
				
				
			}
		}

		private void showContents(DirectoryNode root, String space) {
			
			Iterator iter = root.getEntries();
			while (iter.hasNext()){
				
				Entry entry = (Entry)iter.next();
				resultArea.append(space);
				resultArea.append(entry.getName());
				resultArea.append("\n");
				if (entry instanceof DirectoryEntry){
					
					showContents((DirectoryNode)entry, space + " ");
				}
			}
		}
	}


	public static void main(String argv[]){
	
		JFrame frame = new JFrame("OLE converter");
		frame.getContentPane().setLayout(new BorderLayout());
		frame.getContentPane().add(new GuiPanel(), BorderLayout.CENTER);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		frame.setSize(800, 600);
		frame.setLocation(100, 100);
		frame.setVisible(true);
		
	
	}
	
}
