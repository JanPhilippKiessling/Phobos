

# put your c-style enum into outputfile (see below), run this script and get a c-style switch over that enum in output

import re
import string


outputfile = open("output.txt", "w")
inputfile = open("input.txt", "r")




InputString = inputfile.read()

# Isaac (?!Asimov) will match 'Isaac ' only if it?s not followed by 'Asimov'.
# (?<= PRECESING_MATCH)

# alles zwischen den {} reinholen
OutputListOfStrings = re.findall(r'(?<={).*(?=}+)', 
						  
						  str(InputString),
						  re.I|re.M|re.DOTALL)

# jede zeile in ein listelement
OutputListOfStrings = re.findall(r'.+?(?<=\n)', 
						  OutputListOfStrings[0],
						  re.I|re.M|re.DOTALL)

#remove empty strings
for str in OutputListOfStrings:
    if str == '' or str == '\n':
        OutputListOfStrings.remove(str)


#delete \n
i = 0
for str in OutputListOfStrings:
    OutputListOfStrings[i] = str.rstrip()                  # endl weg
    OutputListOfStrings[i] = re.sub(r'\s',"", OutputListOfStrings[i])         #alle spaces loeschen
    OutputListOfStrings[i] = re.sub(r'(?<=[=]).*',"", OutputListOfStrings[i]) #alles nach = loeschen, wenn da
    OutputListOfStrings[i] = re.sub(r'=',"", OutputListOfStrings[i])          #alle = loeschen, wenn da
    OutputListOfStrings[i] = re.sub(r',',"",OutputListOfStrings[i])           #alle , raus
    print(OutputListOfStrings[i])
    i+=1
    


#now print the new file
outputfile.write("switch()\n")
outputfile.write("{\n")

for str in OutputListOfStrings:
    outputfile.write("\tcase ")
    outputfile.write(str)
    outputfile.write(":\n")

    outputfile.write("\t{\n")
    outputfile.write("\t\t\n")
    outputfile.write("\t}break;\n\n")

outputfile.write("\tdefault:\n")
outputfile.write("\t{\n")
outputfile.write("\t\t// \n")
outputfile.write("\t}\n")
outputfile.write("}\n")




outputfile.close()
inputfile.close()

