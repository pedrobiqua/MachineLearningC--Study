#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <libfccp/csv.h>

using namespace std;
using namespace io;

template <std::size_t... Idx, typename T, typename R>
bool read_row_help(std::index_sequence<Idx...>, T& row, R& r);

template <std::size_t... Idx, typename T>
void fill_values(std::index_sequence<Idx...>, T& row, std::vector<double>& data);

int main()
{
    // Obs: Usando dataset do iris: https://archive.ics.uci.edu/ml/datasets/iris
    // Preprocessamento dos dados que vão ser aprendidos.
    const string file_path = "iris.csv";
    const string file_path_fix = "iris_fix.csv";
    std::ifstream data_stream(file_path);
    std::string data_string((std::istreambuf_iterator<char>(data_stream)), std::istreambuf_iterator<char>());
    data_string = std::regex_replace(data_string, std::regex("Iris-setosa"), "1");
    data_string = std::regex_replace(data_string, std::regex("Iris-versicolor"), "2");
    data_string = std::regex_replace(data_string, std::regex("Iris-virginica"), "3");
    std::ofstream out_stream(file_path_fix);
    out_stream << data_string;

    // Leitura dos dados
    const uint32_t columns_num = 5;
    CSVReader<columns_num> csv_reader(file_path);

    vector<string> categorical_column;
    vector<double> values;

    using RowType = tuple<double, double, double, double, string>;
    RowType row;

    // TODO: Selecionar uma biblioteca para fazer o treinamento desse dataset

    try {
        bool done = false;
        while (!done) {
            // Le a linha do arquivo e preenche a tupla
            done = !read_row_help(
                std::make_index_sequence<std::tuple_size<RowType>::value>{}, row, csv_reader
            );
            if (!done) {
                categorical_column.push_back(std::get<4>(row));
                fill_values(std::make_index_sequence<columns_num - 1>{}, row, values);
            }
        }
    } catch (const io::error::no_digit& err) {
        // ignore badly formatted samples
        std::cerr << err.what() << std::endl;
    }
}

/*
Le a linha do arquivo e preenche a tupla
*/
template <std::size_t... Idx, typename T, typename R>
bool read_row_help(std::index_sequence<Idx...>, T& row, R& r) {
    return r.read_row(std::get<Idx>(row)...);
}

/*
Preenche o vetor de valores com os valores da tupla
*/
template <std::size_t... Idx, typename T>
void fill_values(std::index_sequence<Idx...>, T& row, std::vector<double>& data) {
    data.insert(data.end(), {std::get<Idx>(row)...});
    std::apply([](auto&&... args) {
        ((std::cout << args << ' '), ...);
    }, row);
    std::cout << std::endl;
}