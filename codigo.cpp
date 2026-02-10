#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<vector<string>> values;
char refChar = '@';
string refString = "";
void leer_txt();
vector<string> get_values(string &);
string replace_sharedStrings(const string &, const string &);
string replace_sheets(const string &templateString,unsigned int &v);
void crearSharedStrings(/*const vector<string>& labels*/const string &datos, const string filename);
int main(){
    //los 25 deben ser variables
    unsigned int numColumnas = 5;
    unsigned int numCasillas = 25;

    string nColums = to_string(numColumnas);
    string nCasillas = to_string(numCasillas);

    string ssTemplateBegin = "<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" count=\"*\" uniqueCount=\"*\">\n";
    string aux = "";

    unsigned int nAstericos = 0;
    for(const auto &c: ssTemplateBegin){
        if(c != '*'){
            aux += c;
        }
        else{
            aux += nCasillas;
            nAstericos++;

        }
    }


    //ssTemplateBegin = "";
    string sharedStrings = "";
    sharedStrings += aux;
    aux = "";


    
    //la palabra scope es el contenido de la celda
    
    string ssTemplateBody = "<si><t>*</t></si>\n";
    leer_txt();
    int vv = 0;
    refChar += numColumnas;

    for(const auto &value : values){
        sharedStrings += replace_sharedStrings(ssTemplateBody,value[0]);
        sharedStrings += replace_sharedStrings(ssTemplateBody,value[1]);
        sharedStrings += replace_sharedStrings(ssTemplateBody,value[2]);
        sharedStrings += replace_sharedStrings(ssTemplateBody,value[3]);
        sharedStrings += replace_sharedStrings(ssTemplateBody,value[4]);
        vv++;
        

    }
    refString += (refChar + to_string(vv));

    
    sharedStrings += aux;


    string ssTemplateEnd = "</sst>";

    sharedStrings += ssTemplateEnd;

    cout<<sharedStrings<<endl;

    cout<<"vv: "<<refString<<endl;

    aux = "";
    string ws1Template = "<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" xmlns:x14ac=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac\" mc:Ignorable=\"x14ac\">\n<dimension ref=\"A1:*\"/>\n<sheetViews>\n<sheetView tabSelected=\"1\" workbookViewId=\"0\">\n<selection activeCell=\"*\" sqref=\"*\"/>\n</sheetView>\n</sheetViews>\n<sheetFormatPr baseColWidth=\"10\" defaultColWidth=\"9.140625\" defaultRowHeight=\"15\" x14ac:dyDescent=\"0.25\"/>\n<cols>\n<col min=\"1\" max=\"1\" width=\"9.7109375\" bestFit=\"1\" customWidth=\"1\"/>\n<col min=\"2\" max=\"2\" width=\"12.42578125\" customWidth=\"1\"/>\n<col min=\"3\" max=\"3\" width=\"16.85546875\" bestFit=\"1\" customWidth=\"1\"/>\n<col min=\"4\" max=\"4\" width=\"19.5703125\" bestFit=\"1\" customWidth=\"1\"/>\n<col min=\"5\" max=\"5\" width=\"59\" bestFit=\"1\" customWidth=\"1\"/>\n</cols>\n<sheetData>";
    aux = replace_sharedStrings(ws1Template,refString);
    string worksheetsSheet1 = aux;

    worksheetsSheet1 += "<row r=\"1\" spans=\"1:5\" x14ac:dyDescent=\"0.25\">\n<c r=\"A1\" s=\"2\" t=\"s\">\n<v>0</v>\n</c>\n<c r=\"B1\" s=\"2\" t=\"s\">\n<v>1</v>\n</c>\n<c r=\"C1\" s=\"2\" t=\"s\">\n<v>2</v>\n</c>\n<c r=\"D1\" s=\"2\" t=\"s\">\n<v>3</v>\n</c>\n<c r=\"E1\" s=\"2\" t=\"s\">\n<v>4</v>\n</c>\n</row>";
    if( vv > 1)
        worksheetsSheet1 += "<row r=\"2\" spans=\"1:5\" x14ac:dyDescent=\"0.25\">\n<c r=\"A2\" s=\"1\" t=\"s\">\n<v>5</v>\n</c>\n<c r=\"B2\" s=\"1\" t=\"s\">\n<v>6</v>\n</c>\n<c r=\"C2\" s=\"1\" t=\"s\">\n<v>7</v>\n</c>\n<c r=\"D2\" s=\"1\" t=\"s\">\n<v>8</v>\n</c>\n<c r=\"E2\" s=\"1\" t=\"s\">\n<v>9</v>\n</c>\n</row>";
    if(vv > 2)
        worksheetsSheet1 += "<row r=\"3\" spans=\"1:5\" x14ac:dyDescent=\"0.25\">\n<c r=\"A3\" s=\"3\" t=\"s\">\n<v>10</v>\n</c>\n<c r=\"B3\" s=\"3\" t=\"s\">\n<v>11</v>\n</c>\n<c r=\"C3\" s=\"3\" t=\"s\">\n<v>12</v>\n</c>\n<c r=\"D3\" s=\"3\" t=\"s\">\n<v>13</v>\n</c>\n<c r=\"E3\" s=\"3\" t=\"s\">\n<v>14</v>\n</c>\n</row>";
    
    string ws1TemplateBody = "<row r=\"*\" spans=\"1:5\" x14ac:dyDescent=\"0.25\">\n<c r=\"*\" t=\"s\">\n<v>*</v>\n</c>\n<c r=\"*\" t=\"s\">\n<v>*</v>\n</c>\n<c r=\"*\" t=\"s\">\n<v>*</v>\n</c>\n<c r=\"*\" t=\"s\">\n<v>*</v>\n</c>\n<c r=\"*\" t=\"s\">\n<v>*</v>\n</c>\n</row>";
    
    if(vv > 3){
        unsigned int v = 15;
        while(v < vv*5){
            worksheetsSheet1 += replace_sheets(ws1TemplateBody,v);

            //cout<<"v: "<<v<<endl;

        }
    }

    worksheetsSheet1 += "</sheetData>\n<pageMargins left=\"0.7\" right=\"0.7\" top=\"0.75\" bottom=\"0.75\" header=\"0.3\" footer=\"0.3\"/>\n<tableParts count=\"1\">\n<tablePart r:id=\"rId1\"/>\n</tableParts>\n</worksheet>";

    cout<<worksheetsSheet1<<endl;
    //"sheet1.xml";
    crearSharedStrings(worksheetsSheet1, "sheet1.xml");
    crearSharedStrings(sharedStrings, "sharedStrings.xml");

    return 0;
}

// Genera el sharedStrings.xml y lo guarda
void crearSharedStrings(/*const vector<string>& labels*/const string &datos, const string filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << " para escribir.\n";
        return;
    }

    // Cabecera XML
    /*file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" "
         << "count=\"" << labels.size() << "\" uniqueCount=\"" << labels.size() << "\">\n";
    */
    file<<datos;

    // Cada label dentro de <si><t>...</t></si>
    //for (const auto& label : labels) {
    //    file << "    <si><t>" << escapeXML(label) << "</t></si>\n";
    //}

    //file << "</sst>\n";
    file.close();

    cout << "Archivo '" << filename << "' creado correctamente.\n";
}

string replace_sheets(const string &templateString,unsigned int &v){
    unsigned int r = ((v-(v%5))/5 + 1);
    unsigned char rChar = 'A';
    
    unsigned int nAstericos = 0;
    string aux = "";
    for(const auto &c : templateString){
        if(c != '*'){
            aux += c;
        }
        else{
            if(nAstericos == 0){
                aux += to_string(r);
                nAstericos++;
                continue;
            }
            bool paridad = ((nAstericos % 2) == 0) ? true : false;
            switch( paridad ){
                case true:
                    aux += to_string(v);
                    v+= 1;
                    break;
                default :
                    aux += (rChar );
                    aux += to_string(r);
                    rChar++;
                    break;
            }
            //aux += cadena;

            nAstericos ++;
            //cout<< nAstericos<<endl;

        }

    }
    //cout<<"aux: "<<aux<<endl;
    return aux;



}


string replace_sharedStrings(const string &templateString,const string &cadena){
    //unsigned int nAstericos = 0;
    string aux = "";
    for(const auto &c : templateString){
        if(c != '*'){
            /*switch(c) {
                case '<': aux += '('; break;//"&lt;"; break;
                case '>': aux += ')'; break;//"&gt;"; break;
                case '&': aux += "&amp;"; break;
                case '"': aux += "&quot;"; break;
                case '\'': aux += "&apos;"; break;
                default: aux += c; break;
            }*/
            
            aux += c;
        }
        else{
            /*switch( nAstericos ){
                case 1:
                    aux += "Scope";
                    break;
                case 2:
                    aux += "IP Address";
                    break;
                case 3:
                    aux += "Client ID";
                    break;
                case 4:
                    aux += "Nombre";
                    break;
                case 5:
                    aux += "Descripción";
                    break;
                default :
                    aux += "KK";
                    break;
            }*/
            aux += cadena;

            //nAstericos ++;

        }

    }
    //cout<<"aux: "<<aux<<endl;
    return aux;
}

void leer_txt(){
    ifstream file("reservaciones_dhcp.csv");
    string str; 
    while (getline(file, str))
    {
        // Process str
        //cout<<str<<endl;
        values.push_back(get_values(str));
    }


}


vector<string> get_values(string &linea){
    vector<string> valores;
    string valor = "";
    for(const auto &c : linea){
        if(c == '"')
            continue;

        if( c != ','){
            char cSafe;
            switch(c){
                case '<':
                    cSafe = '(';
                    break;
                case '>':
                    cSafe = ')';
                    break;
                default:
                    cSafe = c;
                    break;
                    

            }
            valor += cSafe;
        }
        else{
            valores.push_back(valor);
            valor = "";
        }

    }

    valores.push_back(valor);

    /*for(const auto &s : valores){
        cout<<s<<endl;
    }
    cout<<"---------------------------------------"<<endl;*/
    return valores;

}