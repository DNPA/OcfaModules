package nl.klpd.tde.ocfamodule.ole2txt;

import java.util.ArrayList;
import java.util.Iterator;

import org.apache.poi.poifs.filesystem.DirectoryEntry;
import org.apache.poi.poifs.filesystem.DirectoryNode;
import org.apache.poi.poifs.filesystem.Entry;

import nl.klpd.tde.treegraphwalker.TreegraphWalker;

public class OleDocWalker<NodeStateType> extends TreegraphWalker<Entry, NodeStateType, Exception> {

	static private ArrayList<Entry> dummy = new ArrayList<Entry>();
	
	
	@SuppressWarnings("unchecked")
	@Override
	protected Iterator<Entry> getSubNodeIterator(Entry inNode) throws Exception {
		if (inNode instanceof DirectoryNode){
			
			//ArrayList<Entry> subNodes = new ArrayList<Entry>();	
			Iterator<Entry> iterator = (Iterator<Entry>)((DirectoryEntry)inNode).getEntries();
			System.out.println("iterator is "+ iterator);
			if (iterator == null){
				
				
				iterator = dummy.iterator();
				System.out.println("iterator is " + iterator);
			}
			return iterator;
			
		} else {
			
			return dummy.iterator();
		}
	}

	
	
}
