#!/usr/bin/env python
# encoding: utf-8

import os
from os import path

COMPILER_NAME = 'squirrel'

def options(opt):
    opt.load('compiler_c')
    
    opt.add_option('--sq', help='Informa o arquivo de entrada squirrel para ser compilado', dest='sq_input')
    opt.add_option('-o', '--output'
                        , help='Informa o caminho para o arquivo de saida em que sera compilado o arquivo squirrel.'
                        , default='a.out'
                        , dest='sq_output')
    
    opt.recurse('test')
    
def configure(conf):
    conf.load('compiler_c flex bison')
    conf.load('squirrel_tool', tooldir='.')
    #isto talvez só seja necessário no windows (?)
    conf.env.LIB_FLEX = ['fl']
    # muda o nome do arquivo ".h" para y.tab.h
    conf.env.BISONFLAGS = ['--defines=y.tab.h', '-v']
    
    conf.recurse('test')

def build(bld):

    path_datastructs = path.join('libs', 'datastructs-c','src');
    bld.stlib(
        target          = 'datastructs-c',
        name            = 'datastructs-c',
        source          = bld.path.ant_glob(path.join(path_datastructs, '*.c')),
        cflags          = ['--std=gnu99'],
        includes        = path_datastructs,
        export_includes = path_datastructs
        )

    path_commons = path.join('src','commons');
    bld.stlib(
        target          = 'squirrel-commons',
        source          = bld.path.ant_glob(path.join(path_commons,'**', '*.c')),
        includes        = path_commons,
        export_includes = path_commons,
        use             = ['datastructs-c'])

    path_compiler = path.join('src','compiler');
    bld.stlib(
        target          = 'squirrel-compiler',
        source          = bld.path.ant_glob(path.join('src','compiler','**', '*.c')),
        includes        = path_compiler,
        export_includes = path_compiler,
        use             = ['datastructs-c', 'squirrel-commons'])

    bld(
        target   = COMPILER_NAME,
        features = 'c cprogram',
        source   = ['lexer.l','parser.y'],
        includes = ['.'],
        use      = ['FLEX', 'datastructs-c', 'squirrel-compiler', 'squirrel-commons'])

    
    path_language = path.join('src','language');
    bld.stlib(
        target          = 'squirrel-language',
        source          = bld.path.ant_glob(path.join('src','language','**', '*.c')),
        includes        = path_language,
        export_includes = path_language,
        use             = ['datastructs-c', 'squirrel-commons'])
    
    bld.recurse('test');
    
    bld.add_group();
    if(bld.options.sq_input):
        outNode = bld.path.get_src().make_node(bld.options.sq_output);
        bld.program(
            features = 'c cprogram',
            target=outNode,
            source=bld.options.sq_input, 
            use =['squirrel-language']
        )

from waflib.Build import BuildContext

# Criando comando especial 'run' para executar compilador
class RunContext(BuildContext):
        cmd = 'run'
        fun = 'run'
def run(ctx):
    ctx(rule=path.join(ctx.path.get_bld().abspath(), COMPILER_NAME));
    
    
