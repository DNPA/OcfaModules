#! /usr/bin/perl
use Spreadsheet::ParseExcel;
use strict;
if (!(defined $ARGV[0])){

  print "Usage: $0 excel_file";
  
  exit;
}
#global formatter
my $parseExcel = new Spreadsheet::ParseExcel;
print "parsing" . $ARGV[0] . "\n";
my $excelBook = $parseExcel->Parse($ARGV[0]);
my $workSheetNr;
open META, ">metadata";
print META "file=" . $excelBook->{File} . "\n";
print META "author=" . $excelBook->{Author} . "\n";
print META "sheetcount=" . $excelBook->{SheetCount} . "\n";
print META "version=" . $excelBook->{Version} . "\n";
my $containsFormulas = 0;
for ($workSheetNr = 0; $workSheetNr < $excelBook->{SheetCount}; $workSheetNr++){
  
  $containsFormulas = $containsFormulas + printSheet($excelBook->{Worksheet}[$workSheetNr], $workSheetNr);
  print META "WorkSheet" . $workSheetNr . "=" 
    . $excelBook->{Worksheet}[$workSheetNr]->{Name};
  print META "\n";
}
if ($containsFormulas > 0){

  print META "containsformulas=" . $containsFormulas;
}






sub printSheet($$){

  my $workSheet = shift;
  print "workSheet is $workSheet\n";
  my $workSheetNr  = shift;
  print "WorkShetNr is $workSheetNr\n";
  my ($minRow, $maxRow) = $workSheet->RowRange();
  my ($minCol, $maxCol) = $workSheet->ColRange();
  print "minRow is $minRow. maxRow = $maxRow\n";
  print "minCol is $minCol. maxCol; = $maxCol\n";
  my $rowNr;
  my $colNr;
  my $containsFormulas = 0;
  if ($minRow <= $maxRow && $minCol <= $maxCol){
    open SHEET, ">sheet" . $workSheetNr;
    for ($rowNr = $minRow; $rowNr <= $maxRow; $rowNr++){
      
      for ($colNr = $minCol; $colNr <= $maxCol; $colNr++){
	
	my $cell = $workSheet->Cell($rowNr, $colNr);
	if (defined $cell){
	  
	  print SHEET  $cell->Value() . "\t"	;
	  if (($cell->{_KIND}) =~ /Fomula/){
	    
	    $containsFormulas++;
	  }
	}
      }
      print SHEET "\n";
    }
    close SHEET;
    return $containsFormulas;

  }
  else {

    return 0;
  }
}

