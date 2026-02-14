#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <unordered_map>

using namespace std;

vector<vector<string>> values;
vector<string> get_values(string &);

void add_celdas(const string &, const string &);
void leer_txt();

string formatSharedString(const string &, const string &);
string formatwSheet1(const string &, const unsigned int &, const vector<string> &, const vector<string> &, const string &);
string selectValwSheet1(const unsigned int &, const unsigned int &, const vector<string> &, const vector<string> &, const string &, unsigned int &);

// Contadores Excel (sharedStrings)
unsigned int count = 0;        // total strings (incluye repetidos)
unsigned int uniqueCount = 0;  // strings únicos

int totalCount = -1;           // índice global de celda (0..N-1)

map<string, unsigned int> uniqueStrings;          // valor -> índice en sharedStrings
unordered_map<string, unsigned int> celdas;       // "A1" -> índice sharedStrings

string celdaActual = "A1";
string celdaAnterior = "A1";

int main() {
    leer_txt();

    // ---------- sharedStrings: header con números reales ----------
    string sharedStrings =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" "
        "count=\"" + to_string(count) + "\" uniqueCount=\"" + to_string(uniqueCount) + "\">\n";

    string sharedStringsTemplate = R"(
        <si>
            <t>*</t>
        </si>)";

    // IMPORTANTÍSIMO:
    // uniqueStrings es map (orden alfabético), pero el índice sharedStrings debe respetar el orden por índice.
    vector<string> indexToString(uniqueCount);
    for (const auto &kv : uniqueStrings) {
        // kv: (string valor, unsigned int idx)
        if (kv.second < indexToString.size()) {
            indexToString[kv.second] = kv.first;
        }
    }

    for (unsigned int idx = 0; idx < uniqueCount; idx++) {
        sharedStrings += formatSharedString(sharedStringsTemplate, indexToString[idx]);
    }

    sharedStrings += "\n</sst>";

    // Si quieres ver sharedStrings:
    // cout << sharedStrings << "\n\n";

    // ---------- worksheet ----------
    string wSheet1 = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main"
           xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"
           xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
           xmlns:x14ac="http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac"
           xmlns:xr="http://schemas.microsoft.com/office/spreadsheetml/2014/revision"
           xmlns:xr2="http://schemas.microsoft.com/office/spreadsheetml/2015/revision2"
           xmlns:xr3="http://schemas.microsoft.com/office/spreadsheetml/2016/revision3"
           mc:Ignorable="x14ac xr xr2 xr3"
           xr:uid="{00000000-0001-0000-0000-000000000000}">
    <dimension ref="A1:*"/>
    <sheetViews>
        <sheetView tabSelected="1" workbookViewId="0">
            <selection activeCell="A1" sqref="A1"/>
        </sheetView>
    </sheetViews>
    <sheetFormatPr baseColWidth="10" defaultColWidth="8.88671875" defaultRowHeight="14.4" x14ac:dyDescent="0.3"/>
    <cols>
        <col min="1" max="1" width="14" bestFit="1" customWidth="1"/>
        <col min="2" max="2" width="14.6640625" bestFit="1" customWidth="1"/>
        <col min="3" max="3" width="16.44140625" bestFit="1" customWidth="1"/>
        <col min="4" max="4" width="39.5546875" bestFit="1" customWidth="1"/>
        <col min="5" max="5" width="114.21875" bestFit="1" customWidth="1"/>
    </cols>
    <sheetData>)";

    // En tu wSheet1 estabas haciendo un replace de '*' por celdaAnterior; eso no tiene mucho sentido aquí.
    // Lo quito para que no meta basura:
    // wSheet1 = formatSharedString(wSheet1, celdaAnterior);

    string wSheet1Template = R"(
        <row r="*" spans="1:5" x14ac:dyDescent="0.3">
            <c r="*" * t="s">
                <v>*</v>
            </c>
            <c r="*" * t="s">
                <v>*</v>
            </c>
            <c r="*" * t="s">
                <v>*</v>
            </c>
            <c r="*" * t="s">
                <v>*</v>
            </c>
            <c r="*" * t="s">
                <v>*</v>
            </c>
        </row>
    )";

    // ceil((totalCount+1)/5)
    unsigned int totalCells = (totalCount >= 0) ? (unsigned int)(totalCount + 1) : 0;
    unsigned int totalRows = (totalCells + 5 - 1) / 5;

    for (unsigned int i = 1; i <= totalRows; i++) {

        vector<string> vals;
        vector<string> rows;

        // celdas A..E para la fila i
        for (int col = 0; col < 5; col++) {
            char colChar = char('A' + col);
            string cell = string(1, colChar) + to_string(i);
            rows.push_back(cell);
        }

        // style solo en la primera fila (si quieres encabezado)
        string s = (i == 1) ? "s=\"2\"" : "";

        // valores: si no existe la celda, no la insertes accidentalmente con []
        for (int col = 0; col < 5; col++) {
            char colChar = char('A' + col);
            string cell = string(1, colChar) + to_string(i);

            auto it = celdas.find(cell);
            if (it != celdas.end()) {
                vals.push_back(to_string(it->second));
            } else {
                // Si no hay valor para esa celda, Excel igual espera algo si tienes <c ... t="s"><v>...</v></c>.
                // Puedes poner 0 o algún índice existente. Aquí pongo 0 si hay al menos 1 shared string, si no, 0.
                vals.push_back("0");
            }
        }

        wSheet1 += formatwSheet1(wSheet1Template, i, vals, rows, s);
    }

    wSheet1 += R"(
    </sheetData>
    <pageMargins left="0.7" right="0.7" top="0.75" bottom="0.75" header="0.3" footer="0.3"/>
</worksheet>)";

   for(const auto &uS: uniqueStrings){
        cout<<uS.first<<endl;
   }

    return 0;
}

// ---------- Formateo de template de sheet ----------
string selectValwSheet1(const unsigned int &nAsteriscos,
                        const unsigned int &r,
                        const vector<string> &vals,
                        const vector<string> &rows,
                        const string &s,
                        unsigned int &conteo) {
    // 1er '*' => número de fila
    if (nAsteriscos == 0) return to_string(r);

    // Después, cada celda tiene 3 '*': r="*", (style '*'), <v>*</v>
    // Patrón: (rRef, style, value)
    unsigned int mod = (nAsteriscos - 1) % 3;

    if (mod == 0) {
        // r="A1"
        return rows[conteo];
    } else if (mod == 1) {
        // style (puede ser "" o 's="2"')
        return s;
    }

    // mod == 2 => valor
    conteo++;
    return vals[conteo - 1];
}

string formatwSheet1(const string &wS1Template,
                     const unsigned int &r,
                     const vector<string> &vals,
                     const vector<string> &rows,
                     const string &s) {
    string newVal;
    newVal.reserve(wS1Template.size() + 64);

    unsigned int nAsteriscos = 0;
    unsigned int conteo = 0;

    for (char c : wS1Template) {
        if (c == '*') {
            newVal += selectValwSheet1(nAsteriscos, r, vals, rows, s, conteo);
            nAsteriscos++;
        } else {
            newVal += c;
        }
    }
    return newVal;
}

// ---------- Formateo simple: reemplaza '*' por val ----------
string formatSharedString(const string &sStemplate, const string &val) {
    string newVal;
    newVal.reserve(sStemplate.size() + val.size());

    for (char c : sStemplate) {
        if (c == '*') newVal += val;
        else newVal += c;
    }
    return newVal;
}

// ---------- Lectura CSV ----------
void leer_txt() {
    ifstream file("reservaciones_dhcp.csv");
    string str;
    while (getline(file, str)) {
        values.push_back(get_values(str));
    }
}

// Guarda en celdas el índice de sharedStrings para esa celda
void add_celdas(const string &nameC, const string &val) {
    auto it = uniqueStrings.find(val);
    if (it == uniqueStrings.end()) {
        // No debería pasar si lo llamas después de try_emplace; pero por seguridad:
        celdas[nameC] = 0;
    } else {
        celdas[nameC] = it->second;
    }
}

// Parser basado en comillas "..." (tu formato actual)
vector<string> get_values(string &linea) {
    vector<string> valores;
    string valor;
    bool enComillas = false;
    bool cadenaVacia = true;

    for (char c : linea) {
        if (c == '"') {
            enComillas = !enComillas;

            if (!enComillas) {
                // Cerró comillas: terminó un campo real
                totalCount++;

                // calcular celda a partir de totalCount (0-based)
                int idx = totalCount;           // 0..N
                int col = idx % 5;              // 0..4 => A..E
                int row = idx / 5 + 1;          // 1..∞
                string cell = string(1, char('A' + col)) + to_string(row);
                celdaActual = cell;

                // sanitizar < >
                for (char &ch : valor) {
                    if (ch == '<') ch = '(';
                    else if (ch == '>') ch = ')';
                }

                // asignar/crear índice en sharedStrings
                auto [it, inserted] = uniqueStrings.try_emplace(valor, uniqueCount);
                if (inserted) uniqueCount++;

                // celdas apunta al índice de sharedStrings
                add_celdas(celdaActual, valor);

                count++; // total strings (incluye repetidos)

                // reset
                valor.clear();
                cadenaVacia = true;
            } else {
                // abrió comillas: empieza valor
                valor.clear();
                cadenaVacia = true;
            }

            continue;
        }

        if (enComillas) {
            if (c != ',') {
                cadenaVacia = false;
                valor += c;
            }
        } else {
            // fuera de comillas: solo para llenar vector si quieres
            if (c == ',') {
                valores.push_back(valor);
                valor.clear();
            }
        }
    }

    // Empuja lo que haya (no es super útil en tu lógica, pero lo dejo)
    valores.push_back(valor);
    return valores;
}
