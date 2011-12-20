package nl.klpd.tde.ocfamodule.ole2txt;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.Reader;
import java.io.StringReader;
import java.io.StringWriter;

import org.apache.poi.hssf.eventusermodel.HSSFEventFactory;
import org.apache.poi.hssf.eventusermodel.HSSFListener;
import org.apache.poi.hssf.eventusermodel.HSSFRequest;
import org.apache.poi.hssf.record.Record;
import org.apache.poi.hssf.usermodel.HSSFCell;
import org.apache.poi.hssf.usermodel.HSSFRow;
import org.apache.poi.hssf.usermodel.HSSFSheet;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;
import org.apache.poi.poifs.filesystem.DirectoryEntry;
import org.apache.poi.poifs.filesystem.DirectoryNode;
import org.apache.poi.poifs.filesystem.POIFSFileSystem;
import org.apache.poi.ss.usermodel.Cell;
import org.apache.poi.ss.usermodel.CellValue;
import org.apache.poi.ss.usermodel.FormulaEvaluator;

public class Excel2Txt {

	HSSFWorkbook workBook = null;
	FormulaEvaluator evaluator ;
	
	public Excel2Txt(FileInputStream stream) throws IOException {
		this(new POIFSFileSystem(stream));
	}

	
	public Excel2Txt(POIFSFileSystem poifs) throws IOException{
		this(poifs, poifs.getRoot());		
	}
	
	
	public Excel2Txt(POIFSFileSystem poifs, DirectoryNode entry) throws IOException{
		
		workBook = new HSSFWorkbook(entry, poifs, false);
        evaluator = workBook.getCreationHelper().createFormulaEvaluator();
        System.out.println("done.");
	}
	
	

	public int getAmountOfSubFiles(){
		
		if (workBook == null){
			
			return 0;
		} else {
			
			return workBook.getNumberOfSheets();
		}
		
		
	}
	
	
	
	public String getSubPageContent(int inPageNr){
	
		return convertToText(workBook.getSheetAt(inPageNr));
	}
	
	public String getSubPageName(int inPageNr){
		
		return workBook.getSheetName(inPageNr);
	}

	private String convertToText(HSSFSheet inSheet) {
		
		if (inSheet.getPhysicalNumberOfRows() == 0){
			
			return "";
		}
		StringBuffer sheetText = new StringBuffer();
		for(int rowNr = inSheet.getFirstRowNum(); rowNr <= inSheet.getLastRowNum(); rowNr++){
			
			HSSFRow row = inSheet.getRow(rowNr);
			if (row != null){
			for(int colNr = 0; colNr < row.getLastCellNum(); colNr++){
				
				HSSFCell cell = row.getCell(colNr);
				if (cell == null){
					
					sheetText.append(' ');
				} else {
					sheetText.append(getCellStringValue(cell));
				}
				sheetText.append("\t");
			}
			}
			sheetText.append("\n");
			
		}
	
		
		return sheetText.toString();
		
		
		
	}

	private String getCellStringValue(HSSFCell cell) {
		switch(cell.getCellType()){
		
		case Cell.CELL_TYPE_BOOLEAN:
			return Boolean.toString(cell.getBooleanCellValue());
			
		case Cell.CELL_TYPE_BLANK:
			return " ";
			
		case Cell.CELL_TYPE_ERROR:
			return "#ERROR#";
			
		case Cell.CELL_TYPE_NUMERIC:
			return Double.toString(cell.getNumericCellValue());
			
		case Cell.CELL_TYPE_STRING:
			return cell.getStringCellValue();
			
		case Cell.CELL_TYPE_FORMULA:
			// TODO find out if something like evaluateincell can be used.
			CellValue cellValue = evaluator.evaluate(cell);
			switch (cellValue.getCellType()){
			
			case Cell.CELL_TYPE_BOOLEAN:
				return Boolean.toString(cell.getBooleanCellValue());
				
			case Cell.CELL_TYPE_BLANK:
				return " ";
				
			case Cell.CELL_TYPE_ERROR:
				return "#ERROR#";
				
			case Cell.CELL_TYPE_NUMERIC:
				return Double.toString(cell.getNumericCellValue());
				
			case Cell.CELL_TYPE_STRING:
				return cell.getStringCellValue();
			default:
				return "#UNKNOWN#";
			}
		default:
			return "#UNKNOWN#";
		
		
		}
	}
	
	
	
}
