#   ============================================================
#   simple test steps re-numbering script
#   ============================================================
#   created by Manfred Sorgo

import re
from sys import argv, exit
from os.path import basename

rxWithHead = re.compile(r'^(( *)//[^\n]*(?:\n\2//[^\n]*)*\n\2TEST *\([^\n]*\n)(\2\{\n.*?\n\2\})', re.M | re.S)
rxJustTest = re.compile(r'^(( *)TEST *\([^\n]*\n)(\2\{\n.*?\n\2\})', re.M | re.S)

class Test(object):
    def __init__(self, ind, head, body):
        self.ind = ind        
        rdStep = r'STEP *\( *(\d+) *\)'
        rxHead = re.compile(r'^(' + ind + r'//!? +)' + rdStep, re.M)
        rxBody = re.compile(r'^(' + ind + r' +)' + rdStep, re.M)

        self.step = 0
        self.head = rxHead.sub(self.repl, head)
        self.step = 0
        self.body = rxBody.sub(self.repl, body)
    
    def repl(self, mo):
        self.step += 1
        return mo.group(1) + f'STEP({self.step})'

    def txt(self):
        return self.head + self.body

class TestSteps(object):
    def __init__(self, fp):
        self.tests = list()

        try:
            with open(fp, 'r') as fh:
                fCont = fh.read()
                for fnd in rxWithHead.findall(fCont):
                    self.tests.append(Test(fnd[1], fnd[0], fnd[2]))
                for fnd in rxJustTest.findall(fCont):
                    self.tests.append(Test(fnd[1], fnd[0], fnd[2]))
                fh.close()
                nCont = rxJustTest.sub(self.repl, rxWithHead.sub(self.repl, fCont))
                if nCont != fCont:
                    with open(fp, 'w') as fh:
                        print(f'-> {basename(fp)}')
                        fh.write(nCont.strip() + '\n')
        except:
            print('error:', fp)
    
    def repl(self, mo):
        return self.tests.pop(0).txt()

def usage(args):
    if '-h' in args:
        print(f'usage: {basename(__file__)} source files to re-number')
        exit()

if __name__ == '__main__':
    usage(argv)
    for fp in argv[1:]:
        TestSteps(fp)
#   ============================================================
#   simple test steps re-numbering script
#   ============================================================
#   created by Manfred Sorgo

import re
from sys import exit
from os.path import basename, isfile

# $1 head
# $2 indent 
# $3 test
# replace by <<n>>$3
# object: Head($)
rxHead = re.compile(r'^(( *)//[^\n]*(?:\n\2//[^\n]*)+)(\n\2TEST\b)', re.M)

# $1 function
# $2 indent
# $3 body
# replace by $1<<n>>
# object: SubStep($3)
rxFunc = re.compile(r'^(( *)(?:TEST|(?:\w+ +)+\w+) *\([^\n]*\n\2)(\{\n.*?\n\2\})', re.M | re.S)

# step replacement
rdStep = r'STEP *\( *\d+ *\)'
rxStepFunc = re.compile(r'^( *)' + rdStep, re.M)
rxStepHead = re.compile(rf'^( *//!? +)' + rdStep, re.M)

# indent matching
rxSubStep = re.compile(r'^( *)(?:SUB|BEGIN)STEPS\b', re.M)
rxEndStep = re.compile(r'^( *)ENDSTEPS\b', re.M)

# $1 begin
# $2 indent
# $3 body
# $4 end
# replace by $1<<n>>$4
# object: SubStep($3)
rxReplSub = re.compile(rf'^(( *)(?:SUB|BEGIN)STEPS\b)(.*?)(\n\2ENDSTEPS)', re.M | re.S)

# restore data
rxUnRepl = re.compile(r'<<(\d+)>>')

class Counter(object):
    def __init__(self, *args):
        self.data = {}
        for arg in args:
            self.data[arg] = self.data.setdefault(arg, 0) + 1
    def __eq__(self, other):
        return False if type(other) != type(self) else self.data == other.data

class replStep(object):
    def nextStep(self, mo):
        self.step += 1
        return mo.group(1) + f'STEP({self.step})'
    
    def doSteps(self, cont, rx:re.Pattern):
        self.step = 0
        return rx.sub(self.nextStep, cont)

class Head(replStep):
    def __init__(self, cont):
        self.cont = self.doSteps(cont, rxStepHead)
    def txt(self):
        return self.cont

class SubStep(replStep):
    def __init__(self, cont):
        self.data = []    
        cont = rxReplSub.sub(self.replSub, cont)
        cont = self.doSteps(cont, rxStepFunc)
        self.cont = rxUnRepl.sub(self.unReplSub, cont)

    def replSub(self, mo):
        ret = f'{mo.group(1)}<<{len(self.data)}>>{mo.group(4)}'
        self.data.append(SubStep(mo.group(3)))
        return ret

    def unReplSub(self, mo):
        return self.data[int(mo.group(1))].txt()

    def txt(self):
        return self.cont

class TestSteps(object):
    def __init__(self, fp):
        print(f'{basename(fp)}:', end = ' ')
        if not isfile(fp):
            print('NOT FOUND')
            return
        with open(fp, 'r') as fh:
            cont = fh.read()
            fh.close()
            if '\t' in cont:
                print('ERROR: TABs found')
                return
            if Counter(*rxSubStep.findall(cont)) != Counter(*rxEndStep.findall(cont)):
                print('ERROR: SUBSTEPS / ENDSTEPS indent mismatch')
                return

            self.data = []
            nCont = rxHead.sub(self.replHead, cont)
            nCont = rxFunc.sub(self.replFunc, nCont)
            nCont = rxUnRepl.sub(self.unRepl, nCont)

            if nCont != cont:
                with open(fp, 'w') as fh:
                    print('written')
                    fh.write(nCont.strip() + '\n')
            else:
                print('unchanged')

    def replHead(self, mo):
        ret = f'<<{len(self.data)}>>{mo.group(3)}'
        self.data.append(Head(mo.group(1)))
        return ret
    
    def replFunc(self, mo):
        ret = f'{mo.group(1)}<<{len(self.data)}>>'
        self.data.append(SubStep(mo.group(3)))
        return ret
    
    def unRepl(self, mo):
        return self.data[int(mo.group(1))].txt()

def usage(args):
    if '-h' in args:
        print(f'usage: {basename(__file__)} source files to re-number')
        exit()

if __name__ == '__main__':
    from sys import argv
    usage(argv)
    for fp in argv[1:]:
        TestSteps(fp)
