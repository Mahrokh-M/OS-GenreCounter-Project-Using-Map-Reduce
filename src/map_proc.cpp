#include "../includes/map_proc.hpp"

using namespace std;

MapProc::MapProc(string dataLine_) {
    dataLine = dataLine_;
}

void MapProc::startMap() {
    // Extract file path, part number, and genres data from dataLine
    string partDir = Utils::parse_line(dataLine, '#');
    string partNum = Utils::parse_line(dataLine, '#');
    string genersData = Utils::parse_line(dataLine, '#');
    
    // Read file content
    string fileData = Utils::read_from_file(partDir);

    // Convert genres data into a list
    vector<string> genersList = Utils::make_list_from_line(genersData, ',');

    // Initialize the genre count table
    vector<int> countTable(genersList.size(), 0);

    // Split file content into lines
    vector<string> partLines = Utils::make_list_from_line(fileData, '\n');

    // Process each line in the file
    for (unsigned int i = 0; i < partLines.size(); i++) {
        string bookName = Utils::parse_line(partLines[i], ',');
        vector<string> bookGeners = Utils::make_list_from_line(partLines[i], ',');
        for (unsigned int j = 0; j < bookGeners.size(); j++) {
            int index = Utils::get_index(bookGeners[j], genersList);
            if (index >= 0) {
                countTable[index]++;
            }
        }
    }

    // Write results into FIFOs
    char buff_[MAX_SIZE_STR];
    int write_fifo;
    for (unsigned int i = 0; i < countTable.size(); i++) {
        string dir = Utils::make_pipe_dir(genersList[i], stoi(partNum));
        write_fifo = open(&dir[0], O_WRONLY);
        strcpy(buff_, to_string(countTable[i]).c_str());
        write(write_fifo, buff_, MAX_SIZE_STR);
        close(write_fifo);
    }
}

