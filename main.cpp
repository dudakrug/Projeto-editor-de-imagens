#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <vector>

using namespace std;

#pragma pack(push, 1)

struct CabecalhoBMP
{
    uint16_t tipo;
    uint32_t tamanhoArquivo;
    uint16_t reservado1;
    uint16_t reservado2;
    uint32_t offset;
};

struct CabecalhoImagem
{
    uint32_t tamanho;
    int32_t largura;
    int32_t altura;
    uint16_t planos;
    uint16_t bitsPorPixel;
    uint32_t compressao;
    uint32_t tamanhoImagem;
    int32_t larguraPixel;
    int32_t alturaPixel;
    uint32_t cores;
    uint32_t coresImportantes;
};

#pragma pack(pop)


// struct para armazenar as informacoes da imagem
struct ImagemInterna
{
    int largura;
    int altura;
    vector<unsigned char> pixels;
};

//conv gray

void ConvGrey(ImagemInterna imagem)
{
    for (int i = 0; i < imagem.pixels.size(); i += 3)
    {
        unsigned char R = imagem.pixels[i];
        unsigned char G = imagem.pixels[i + 1];
        unsigned char B = imagem.pixels[i + 2];

        unsigned char gray = 0.30 * R + 0.59 * G + 0.11 * B;

        imagem.pixels[i] = gray;
        imagem.pixels[i + 1] = gray;
        imagem.pixels[i + 2] = gray;
    }
}

//Pegando valor json

string pegarValor(string json, string nome)
{
    string procurar = "\"" + nome + "\":";

    int inicio = json.find(procurar);

    if (inicio == -1)
        return "Nome nao encontrado";

    inicio += procurar.length();

    while (json[inicio] == ' ')
        inicio++;

    // String
    if (json[inicio] == '"')
    {
        inicio++;

        int fim = json.find('"', inicio);

        return json.substr(inicio, fim - inicio);
    }

    // Vetor [x, y]
    if (json[inicio] == '[')
    {
        int fim = json.find(']', inicio);

        return json.substr(inicio, fim - inicio + 1);
    }

    // Numero
    int fim = json.find_first_of(",}", inicio);

    return json.substr(inicio, fim - inicio);
}
//lendo json

string lerArquivoJson(string caminhoArquivo)
{
    ifstream arquivo(caminhoArquivo);

    if (!arquivo.is_open())
    {
        cerr << "Erro: arquivo nao encontrado" << endl;
    }

    string conteudo = "";
    string linha;

    while (getline(arquivo, linha))
    {
        conteudo += linha;
    }

    arquivo.close();

    return conteudo;
}

//recorte da imagem

ImagemInterna corta(ImagemInterna original, int x, int y,
                    int novaLargura, int novaAltura)
{
    ImagemInterna recorte;

    recorte.largura = novaLargura;
    recorte.altura = novaAltura;

    recorte.pixels.resize(novaLargura * novaAltura * 3);

    for (int linha = 0; linha < novaAltura; linha++)
    {
        int inicioOriginal =
            ((y + linha) * original.largura + x) * 3;

        int inicioRecorte =
            linha * novaLargura * 3;

        for (int i = 0; i < novaLargura * 3; i++)
        {
            recorte.pixels[inicioRecorte + i] =
                original.pixels[inicioOriginal + i];
        }
    }

    return recorte;
}


int main()
{
    setlocale(LC_ALL, "");

    //leitura do json

    string json = lerArquivoJson("comandos.mpi");

    //Pegando os cmd's

    string comando = pegarValor(json, "cmd");


    ImagemInterna imagemOriginal;


    //Abrindo a imagem
    if (comando == "Abra")
    {
        string nomeArquivo =
            pegarValor(json, "arq");

        ifstream arquivoImagem;

        arquivoImagem.open(
            nomeArquivo,
            ios::in | ios::binary
        );

        if (!arquivoImagem.is_open())
        {
            cerr << "Erro: nao foi possivel abrir a imagem" << endl;
            return -1;
        }

        //Leitura do cabecalho
        CabecalhoBMP cabecalho;
        CabecalhoImagem cabecalhoImagem;


        // pula os 14 bytes do primeiro cabecalho
        for (int i = 0; i < 14; i++)
            arquivoImagem.get();


        // pula o tamanho do segundo cabecalho
        for (int i = 0; i < 4; i++)
            arquivoImagem.get();


        //Funcao para ler largura
        cabecalhoImagem.largura = 0;

        for (int i = 0; i < 4; i++)
        {
            cabecalhoImagem.largura +=
                arquivoImagem.get() << (i * 8);
        }

        //Funcao de leitura da altura
        cabecalhoImagem.altura = 0;

        for (int i = 0; i < 4; i++)
        {
            cabecalhoImagem.altura +=
                arquivoImagem.get() << (i * 8);
        }


        // pula planos e bits por pixel
        for (int i = 0; i < 4; i++)
            arquivoImagem.get();


        // pula o restante do cabecalho
        for (int i = 0; i < 24; i++)
            arquivoImagem.get();


        //Tranformando a imagem original
        imagemOriginal.largura =
            cabecalhoImagem.largura;

        imagemOriginal.altura =
            cabecalhoImagem.altura;


        imagemOriginal.pixels.resize(
            imagemOriginal.largura *
            imagemOriginal.altura *
            3
        );


    //Lendo os pixels
        for (int i = 0; i < imagemOriginal.pixels.size(); i++)
        {
            imagemOriginal.pixels[i] =
                arquivoImagem.get();
        }


        arquivoImagem.close();
    }

    //Funcao para recorte
    if (comando == "Recorta")
    {
        ImagemInterna imagemRecortada = corta(
            imagemOriginal,
            100,   // x
            50,    // y
            300,   // largura
            150    // altura
        );
    }
    if (comando == "GravaBMP")
    {
        GravaBMP(imagemRecortada, "recorte.bmp");
    }


    // Comando ConvGray
    if (comando == "ConvGray")
    {
        ImagemInterna imagemCinza = imagemRecortada;

        imagemCinza = ConvGray(imagemCinza);
    }


    // Comando GravaBMP novamente
    if (comando == "GravaBMP")
    {
        GravaBMP(imagemCinza, "recorte_cinza.bmp");
    }

    return 0;
}