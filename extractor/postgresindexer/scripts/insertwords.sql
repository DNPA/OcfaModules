create table words (wordid serial,
		word varchar(255),
		unique (word));
	
create unique index wordindex on words (word);
 
create table hits (wordid integer, dataid integer);
create language plpgsql;

create or replace function insertword(inWord varchar, inDataId integer) returns integer AS '
DECLARE
     theWordId integer;
BEGIN
	select wordid into theWordId from words where word = inWord;
	if not found then
	
		insert into words (word) VALUES (inWord);
		select wordid into theWordId from words where word = inWord;

	end if;
	insert into hits(wordid, dataid) values(theWordId, inDataId);
	return 1;
END' language plpgsql