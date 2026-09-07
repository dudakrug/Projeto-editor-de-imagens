#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <vector>

using namespace std;

//struct para armazenar as informacoes da imagem
struct ImagemInterna{
    int largura;
    int altura;
    vector<unsigned char> pixels; //R, G, B de cada pixel
};

//funcao para recortar a imagem
ImagemInterna corta(ImagemInterna original, int x, int y, int novaLargura, int novaAltura)
{
    ImagemInterna recorte;
    recorte.largura = novaLargura;
    recorte.altura = novaAltura;

    //preencher o vetor de pixels com o tamanho correto
    recorte.pixels.resize(novaLargura * novaAltura * 3);

    //for para controlar a altura do recorte
    for (int linha = 0; linha < novaAltura; linha++)
    {
        //calcular o inicio da linha na imagem original e no recorte
        int inicioOriginal = ((y + linha) * original.largura + x) * 3;
        int inicioRecorte = linha * novaLargura * 3;

        //percorrer os bytes da linha e copiar para o vetor de pixels do recorte
        for (int i = 0; i < novaLargura * 3; i++)
            recorte.pixels[inicioRecorte + i] = original.pixels[inicioOriginal + i];
    }

    return recorte;
}

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
    ifstream arquivo(nomeArquivo, ios::in | ios::binary);

    if(!arquivo.is_open()){
        cerr << "Nao foi possivel ler o arquivo!" << endl;
        return -1;
    }

    //cria a imagem original
    ImagemInterna imagemOriginal;

    arquivo.close();

    //cria a copia da imagem original
    ImagemInterna imagemCopia = imagemOriginal;

    //valores ficticios para teste
    ImagemInterna imagemRecortada = corta(imagemCopia, 10, 10, 100, 100);

    return 0;
}
