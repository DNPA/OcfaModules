divert(-1)
include(`extractor.tmpl.m4')dnl
include(`dissector.tmpl.m4')dnl
include(`makefile.tmpl.m4')dnl
define(moduletype,  translit(moduletype, `a-z', `A-Z'))
define(classname, translit(substr(modulename,0,1),`a-z',`A-Z')`'substr(modulename, 1, eval(len(modulename)-1)))dnl
ifdef(`HPP', `define(`WRAPPER', moduletype`HPP($@)')', `define(`WRAPPER', moduletype`CPP($@)')')dnl
ifdef(`MAKE', `define(`WRAPPER', `MAKEFILE($@)')')dnl
divert(0)dnl
WRAPPER(modulename, translit(modulename, `a-z', `A-Z'), classname)dnl
