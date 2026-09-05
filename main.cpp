#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <vector>

using namespace std;

struct ImagemInterna{
    int largura;
    int altura;
    vector<unsigned char> pixels; //R, G, B de cada pixel
};

int main()
{
    //setar caracteres em portugues
    setlocale(LC_ALL, "");

    //[INPUT]

    //POLIR ESTA MERDA
    string nomeArquivo;
    cout << "Digite o nome do arquivo BMP: " << endl;
    getline(cin, nomeArquivo);

    //[PROCESS]

    //funcao para ler o arquivo em binario
    ifstream arquivo(nomeArquivo, ios::binary);

    if(!arquivo.is_open()){
        cerr << "Nao foi possivel ler o arquivo!" << endl;
        return -1;
    }

    ImagemInterna imagemOriginal;

    arquivo.close();

    ImagemInterna imagemCopia = imagemOriginal;

    return 0;
}
