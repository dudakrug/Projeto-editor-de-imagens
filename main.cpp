#include <iostream>
#include <fstream>
#include <string>
#include <locale>

using namespace std;

int main()
{
    //setar caracteres em portugues
    setlocale(LC_ALL, "");

    //[INPUT]

    //POLIR ESTA MERDA
    string nome_arquivo;
    cout << "Digite o nome do arquivo.txt: " << endl;
    getline(cin, nome_arquivo);


    //[PROCESS]

    //funcao para ler arquivos
    ifstream arquivo(nome_arquivo);

    if(!arquivo.is_open()){
        cerr << "Não foi possível ler o arquivo!" << endl;
        return -1;
    }



    return 0;
}
