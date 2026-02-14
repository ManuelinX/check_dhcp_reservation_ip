#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

vector<vector<string>> values;
vector<string> get_values(string &);

void add_celdas(const string &, const string &);
void leer_txt();
string formatSharedString(const string &,const string &);
string formatwSheet1(const string &,const unsigned int &,const vector<string> &,const vector<string> &, const string &);

void crearXML(/*const vector<string>& labels*/const string &datos, const string filename);

void movFile(const string &, const string &);

unsigned int countXML = 0;
unsigned int uniqueCount = 0;
int totalCount = -1;
//unordered_map<string,unsigned int> uniqueStrings;
unordered_map<string,unsigned int> uniqueStrings;
unordered_map<string,unsigned int> celdas;
string celdaActual = "A1";
string celdaAnterior = "A1";



int main(){
    leer_txt();
    string sharedStrings = "";

    vector<pair<string,unsigned int>> vec (uniqueStrings.begin() , uniqueStrings.end());

    sort(vec.begin(),vec.end(),[](const auto& a, const auto& b) {
            return a.second < b.second;  // ascendente
        });
    string sharedStringsTemplate = R"(
        <si>
            <t>*</t>
        </si>)";
    
    for(const auto &[key,vval]: vec){
        sharedStrings += formatSharedString(sharedStringsTemplate,key);
    }
    string auxSS = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <sst xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" count=")";
    auxSS += to_string(totalCount+1) + R"(" uniqueCount=")";
    auxSS += to_string(uniqueCount) +R"(">)";
    

    sharedStrings += "\n</sst>";

    auxSS += sharedStrings;
    sharedStrings = auxSS;
    /*
    xmlns:xr="http://schemas.microsoft.com/office/spreadsheetml/2014/revision" xmlns:xr2="http://schemas.microsoft.com/office/spreadsheetml/2015/revision2" xmlns:xr3="http://schemas.microsoft.com/office/spreadsheetml/2016/revision3"
    mc:Ignorable="x14ac xr xr2 xr3"
    xr:uid="{00000000-0001-0000-0000-000000000000}"
    */

    string wSheet1 = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006" xmlns:x14ac="http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac" mc:Ignorable="x14ac">
    <dimension ref="A1:*"/>
    <sheetViews>
    <sheetView tabSelected="1" workbookViewId="0">
    <selection activeCell="E1" sqref="E1"/>
    </sheetView>
    </sheetViews>
    <sheetFormatPr baseColWidth="10" defaultColWidth="8.85546875" defaultRowHeight="15" x14ac:dyDescent="0.25"/>
    <cols>
    <col min="1" max="1" width="14" bestFit="1" customWidth="1"/>
    <col min="2" max="2" width="14.7109375" bestFit="1" customWidth="1"/>
    <col min="3" max="3" width="16.42578125" bestFit="1" customWidth="1"/>
    <col min="4" max="4" width="39.5703125" bestFit="1" customWidth="1"/>
    <col min="5" max="5" width="114.28515625" bestFit="1" customWidth="1"/>
    </cols>
    <sheetData>)";
    wSheet1 = formatSharedString(wSheet1,celdaAnterior);
    string wSheet1Template = R"(
        <row r="*" spans="1:5" x14ac:dyDescent="0.25">
            <c r="*"* t="s">
                <v>*</v>
            </c>
            <c r="*"* t="s">
                <v>*</v>
            </c>
            <c r="*"* t="s">
                <v>*</v>
            </c>
            <c r="*"* t="s">
                <v>*</v>
            </c>
            <c r="*"* t="s">
                <v>*</v>
            </c>
        </row>
    )";

    unsigned int totalRows = (totalCount + 1)/5;
    char nameRow = 'A';
 
    for(int i = 1; i<= totalRows; i++){
        
        vector<string> vals; //{to_string(i)};
        vector<string> rows;
        for(int j = 0; j<5; j++){
            string auxCelda = (nameRow + to_string(i));
            rows.push_back(auxCelda);
            nameRow = (nameRow - 65) + 1;
            nameRow %= 5;
            nameRow += 65;
        }
        string s;
        if(i-1 == 0){
            //s = " s=\"2\"";
            s= "";
        }
        else{
            s= "";

        }
        nameRow = 'A';
        for(int j = 0; j<5; j++){
            string auxCelda = nameRow + to_string(i);
            vals.push_back(to_string(celdas[auxCelda]));
            nameRow = (nameRow - 65) + 1;
            nameRow %= 5;
            nameRow += 65;
        }
        /*
        for(int j = 0; j<5; j++){

            vals.push_back(nameRow);
            nameRow = (nameRow - 65) + 1;
            nameRow %= 5;
            nameRow += 65;
        }*/

        wSheet1 += formatwSheet1(wSheet1Template,i,vals,rows,s);

        


    } 

    
    wSheet1 += R"(
        </sheetData>
        <pageMargins left="0.7" right="0.7" top="0.75" bottom="0.75" header="0.3" footer="0.3"/>
    </worksheet>)";
    cout<<wSheet1<<endl;

    crearXML(wSheet1, "sheet1.xml");
    crearXML(sharedStrings, "sharedStrings.xml");

    for(const auto &v: uniqueStrings){
        cout<<"index: "<<v.first<<" val: "<<v.second<<endl;
    }
    
    
    movFile("sharedStrings.xml","template/xl");
    movFile("sheet1.xml","template/xl/worksheets");
    //cout<<celdaAnterior<<endl;
    return 0;
}

void movFile(const string &archivo, const string &ruta){
    fs::path archivo_origen = archivo;
    fs::path carpeta_destino = ruta;
    fs::path archivo_destino = carpeta_destino / archivo_origen.filename();
    try{
        if(!fs::exists(carpeta_destino)){
            fs::create_directories(carpeta_destino);
        }

        fs::rename(archivo_origen,archivo_destino);


    }
    catch(const fs::filesystem_error & e){
        if(fs::exists(archivo_destino)){
            fs::remove(archivo_destino);
            fs::rename(archivo_origen,archivo_destino);
            cout<<"Archivo reemplazado correctamente..."<<endl;
        }
        else{
            cerr<<"Error al mover el archivo"<<e.what()<<endl;
        }
    }
}

// Genera el sharedStrings.xml y lo guarda
void crearXML(/*const vector<string>& labels*/const string &datos, const string filename) {
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


string selectValwSheet1(const unsigned int &nAsteriscos,const unsigned int &r , const vector<string> & vals,  const vector<string> &rows, const string &s,unsigned int &conteo){
    if(nAsteriscos == 0){
        return to_string(r);
    }
    
    if(((nAsteriscos-1)%3) == 0){
        //nameRow = (nameRow - 65) + 1;
        //nameRow %= 5;
        //nameRow += 65;
        
        return rows[conteo];
    }
    else if(((nAsteriscos-1)%3) == 1){
        return s;

    }


    conteo ++;
    return vals[conteo-1];

}

string formatwSheet1(const string &wS1Template,const unsigned int &r,const vector<string> &vals,const vector<string> &rows ,const string &s){
    string newVal = "";
    unsigned int nAsteriscos = 0;
    unsigned int conteo = 0;
    for(const auto &c:wS1Template){
        if(c == '*'){
            newVal += selectValwSheet1(nAsteriscos,r,vals,rows,s,conteo);
            nAsteriscos++;
            
        }
        else{
            newVal += c;
        }
    }

    return newVal;
}

string formatSharedString(const string &sStemplate,const string &val){
    string newVal = "";
    for(const auto &c : sStemplate){
        if(c != '*'){
            newVal += c;
        }
        else{
            newVal += val;
        }
    }

    return newVal;
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

void add_celdas(const string &nameC, const string &val){

    if (uniqueStrings.find(val) == uniqueStrings.end()){
    // not found
        celdas.insert({nameC,uniqueCount});
    } 
    else{
        celdas.insert({nameC,uniqueStrings[val]});
    // found
    }




}

vector<string> get_values(string &linea){
    vector<string> valores;
    string valor = "";
    bool cadenaVacia = true;
    bool paridad = true;
    for(const auto &c : linea){
        if(c == '"'){
            paridad ^= true;
            
            if(paridad){
                //celdas.insert({celdaActual,totalCount});
                add_celdas(celdaActual,valor);
                //cout<<celdaActual<<endl;
                celdaAnterior = celdaActual;
                celdaActual = (((celdaActual[0]-65)+1)%5)+65;
                celdaActual += to_string(( 1 + ((totalCount+1)-((totalCount+1)%5))/5));
                

                if(cadenaVacia) continue;

                auto [ite,status] = uniqueStrings.try_emplace(valor,uniqueCount);
                //cout<<paridad<<"-"<<cadenaVacia<<endl;
                uniqueCount = (status)? uniqueCount + 1 : uniqueCount;
                
                
                countXML++;
            }
            else{
                totalCount++;
                valor = "";
                cadenaVacia = true;
                
            }

            continue;
        }

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
            cadenaVacia = false;
            valor += cSafe;
        }
        else{
            valores.push_back(valor);
            
        }

    }

    valores.push_back(valor);

    /*for(const auto &s : valores){
        cout<<s<<endl;
    }
    cout<<"---------------------------------------"<<endl;*/
    return valores;

}

//compilacion estatica: g++ codigo_mejoras.cpp -std=c++17 -static -static-libgcc -static-libstdc++ -o programa.exe