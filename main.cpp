#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <vector>
#include <cstdint>

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

//funcao para converter a imagem para tons de cinza
ImagemInterna ConvGrey(ImagemInterna imagem)
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

    return imagem;
}

//funcao para pegar valores do arquivo json
string PegarValorJson(string json, string nome)
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

//funcao para ler o arquivo json
string LerArquivoJson(string caminhoArquivo)
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

//funcao para recortar a imagem
ImagemInterna Corta(ImagemInterna original, int x, int y, int novaLargura, int novaAltura)
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

//funcao para abrir a imagem
ImagemInterna AbrirImagem(string json, ImagemInterna imagemOriginal){

    string nomeArquivo = PegarValorJson(json, "arq");

    ifstream arquivoImagem;
    arquivoImagem.open(nomeArquivo, ios::in | ios::binary);

    if (!arquivoImagem.is_open()){
        cerr << "Erro: nao foi possivel abrir a imagem" << endl;
        exit(1);
    }

    //leitura dos cabecalhos
    CabecalhoBMP cabecalhoBMP;
    CabecalhoImagem cabecalhoImagem;

    //pula os 14 bytes do primeiro cabecalho
    for (int i = 0; i < 14; i++){
        arquivoImagem.get();
    }
    //pula o tamanho do segundo cabecalho
    for (int i = 0; i < 4; i++){
        arquivoImagem.get();
    }

    //funcao para ler a largura
    cabecalhoImagem.largura = 0;

    for (int i = 0; i < 4; i++){
        cabecalhoImagem.largura += arquivoImagem.get() << (i * 8);
    }

    //funcao para ler a altura
    cabecalhoImagem.altura = 0;

    for (int i = 0; i < 4; i++){
        cabecalhoImagem.altura += arquivoImagem.get() << (i * 8);
    }

    //pula planos e bits por pixel
    for (int i = 0; i < 4; i++){
        arquivoImagem.get();
    }

    //pula o restante do cabecalho
    for (int i = 0; i < 24; i++){
        arquivoImagem.get();
    }

    //Tranformando a imagem original
    imagemOriginal.largura = cabecalhoImagem.largura;
    imagemOriginal.altura = cabecalhoImagem.altura;

    imagemOriginal.pixels.resize(imagemOriginal.largura * imagemOriginal.altura * 3);

    //Lendo os pixels
    for (int i = 0; i < imagemOriginal.pixels.size(); i++){
        imagemOriginal.pixels[i] = arquivoImagem.get();
    }

    arquivoImagem.close();

    return imagemOriginal;
}

int main()
{
    setlocale(LC_ALL, "");

    //leitura do json
    string json = LerArquivoJson("comandos.mpi");

    //separar os valores de cmd´s do arquivo json
    string comando = PegarValorJson(json, "cmd");

    ImagemInterna imagemOriginal;

    //Abrindo a imagem
    if (comando == "Abra"){
        imagemOriginal = AbrirImagem(json, imagemOriginal);
    }


    if (comando == "Recorta")
    {
        ImagemInterna imagemRecortada = Corta(
            imagemOriginal, //arquivo
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

        imagemCinza = ConvGrey(imagemCinza);
    }


    // Comando GravaBMP novamente
    if (comando == "GravaBMP")
    {
        GravaBMP(imagemCinza, "recorte_cinza.bmp");
    }

    return 0;
}