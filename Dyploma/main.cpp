#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <windows.h>
#undef max
#undef min
#include <cstdlib>
#include <thread>
#include <mutex>
#include <map>
#include <cctype>
#include <sstream>
#include <algorithm>
#include <set>


#include "jotaro_kmp.h"
#include "dio_boyer.h"
#include "kakyoin_rk.h"
#include "speedwagon_bitap.h"
#include "chrono_jojo.h"
#include "file_loader.h"
#include "memory_meter.h"
#include "comparison_counter.h"
#include <cstdlib>

bool headerPrinted = false;
std::mutex printMutex;

std::string trimFilename(const std::string& filename) {
    size_t pos = filename.find(".txt");
    return (pos != std::string::npos) ? filename.substr(0, pos) : filename;
}

std::string algoDisplayName(const std::string& algoName) {
    if (algoName == "JotaroKMP") return "Knuth-Morris-Pratt";
    if (algoName == "DioBoyerMoore") return "Boyer-Moore";
    if (algoName == "KakyoinRabinKarp") return "Rabin-Karp";
    if (algoName == "SpeedwagonBitap") return "Bitap";
    return algoName;
}

std::vector<std::string> extractTopNWords(const std::string& text, int N) {
    std::set<std::string> bannedWords = {
        "can", "cant", "feeling", "gonna", "wanna"
    };

    std::map<std::string, int> freq;
    std::string word;

    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            word += std::tolower(static_cast<unsigned char>(c));
        }
        else if (!word.empty()) {
            if (word.length() <= 30 && !bannedWords.count(word)) {
                freq[word]++;
            }
            word.clear();
        }
    }
    if (!word.empty() && word.length() <= 30 && !bannedWords.count(word)) {
        freq[word]++;
    }

    std::vector<std::pair<std::string, int>> sorted(freq.begin(), freq.end());
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });

    std::vector<std::string> result;
    for (int i = 0; i < N && i < static_cast<int>(sorted.size()); ++i) {
        result.push_back(sorted[i].first);
    }
    return result;
}

struct AlgoResult {
    double timeTaken = 0;
    size_t memoryUsed = 0;
    int comparisons = 0;
    size_t matches = 0;
};

void logTheBattle(const std::string& sourceFile, const std::string& pattern, const std::string& standName,
    int textLength, double elapsedTime, size_t memoryUsed, int comparisons,
    const std::vector<int>& hits) {
    std::lock_guard<std::mutex> guard(printMutex);
    if (!headerPrinted) {
        std::cout << "Source          | Pattern         | Algorithm          | TextLen | Time(ms) | Matches | Comparisons\n";
        std::cout << "----------------------------------------------------------------------------------------------------\n";
        headerPrinted = true;
    }
    std::string trimmedPattern = pattern.length() > 15 ? pattern.substr(0, 12) + "..." : pattern;
    printf("%-15s | %-15s | %-18s | %7d | %8.2f | %7zu | %11d\n",
        trimFilename(sourceFile).c_str(), trimmedPattern.c_str(), algoDisplayName(standName).c_str(),
        textLength, elapsedTime, hits.size(), comparisons);
}

void runJojoAlgorithm(const std::string& algoName, const std::string& text, const std::string& pattern,
    const std::string& filename, JojoChrono& chrono, AlgoResult& outResult) {
    JojoComparisonCounter::reset();
    chrono.start();
    std::vector<int> result;

    if (algoName == "JotaroKMP") result = jotaroKMPsearch(text, pattern);
    else if (algoName == "DioBoyerMoore") result = dioBoyerMooreSearch(text, pattern);
    else if (algoName == "KakyoinRabinKarp") result = kakyoinRabinKarpSearch(text, pattern);
    else if (algoName == "SpeedwagonBitap") result = speedwagonBitapSearch(text, pattern);
    else return;

    double timeTaken = chrono.stop();
    size_t memory = getCurrentMemoryKB();
    int comparisons = JojoComparisonCounter::get();

    outResult.timeTaken = timeTaken;
    outResult.memoryUsed = memory;
    outResult.comparisons = comparisons;
    outResult.matches = result.size();

    logTheBattle(filename, pattern, algoName, (int)text.size(), timeTaken, memory, comparisons, result);
}

void threadSearchWorker(const std::string& algoName, const std::string& text, const std::string& pattern,
    const std::string& filename, JojoChrono& chrono, AlgoResult& result) {
    runJojoAlgorithm(algoName, text, pattern, filename, chrono, result);
}

void compareAlgorithmsSummary(const std::string& filename, const std::string& pattern, int textLength,
    const std::map<std::string, AlgoResult>& results) {
    std::lock_guard<std::mutex> guard(printMutex);
    std::cout << "\n[Summary] Comparison for pattern \"" << pattern << "\" in " << trimFilename(filename) << ":\n";
    std::cout << "Algorithm          | Time(ms) | Memory(KB) | Comparisons | Matches | Notes\n";
    std::cout << "-------------------------------------------------------------------------------\n";

    double bestTime = std::numeric_limits<double>::max();
    size_t bestMemory = std::numeric_limits<size_t>::max();
    int bestComparisons = std::numeric_limits<int>::max();

    for (const auto& [algo, res] : results) {
        if (res.timeTaken < bestTime) bestTime = res.timeTaken;
        if (res.memoryUsed < bestMemory) bestMemory = res.memoryUsed;
        if (res.comparisons < bestComparisons) bestComparisons = res.comparisons;
    }

    for (const auto& [algo, res] : results) {
        std::string notes;
        if (res.timeTaken == bestTime) notes += "Fastest ";
        if (res.memoryUsed == bestMemory) notes += "LowestMem ";
        if (res.comparisons == bestComparisons) notes += "LeastComp ";

        printf("%-18s | %8.2f | %10zu | %11d | %7zu | %s\n",
            algoDisplayName(algo).c_str(), res.timeTaken, res.memoryUsed,
            res.comparisons, res.matches, notes.c_str());
    }
    std::cout << "-------------------------------------------------------------------------------\n\n";
}

std::vector<std::string> splitTextWithOverlap(const std::string& text, size_t numParts, size_t overlap) {
    std::vector<std::string> parts;
    size_t partSize = text.size() / numParts;

    for (size_t i = 0; i < numParts; ++i) {
        size_t start = i * partSize;
        size_t end = (i == numParts - 1) ? text.size() : (start + partSize + overlap);
        if (start > 0) start -= overlap;
        if (end > text.size()) end = text.size();
        parts.push_back(text.substr(start, end - start));
    }
    return parts;
}

std::map<std::string, std::map<std::string, double>> allResultsForPlot;


std::map<std::string, std::vector<double>> timesPerAlgorithm;

std::map<std::string, std::map<std::string, std::vector<double>>> timesPerFileAndAlgorithm;


void writeAllDataForPlot() {
    std::ofstream out("plot_data.txt");
    if (!out.is_open()) {
        std::cerr << "Error: Cannot create plot_data.txt\n";
        return;
    }
    

    out << "\"Source (pattern)\" KnuthMorrisPratt BoyerMoore RabinKarp Bitap\n";

    for (const auto& [label, timesMap] : allResultsForPlot) {
        out << "\"" << label << "\" ";
        out << (timesMap.count("JotaroKMP") ? timesMap.at("JotaroKMP") : 0) << " ";
        out << (timesMap.count("DioBoyerMoore") ? timesMap.at("DioBoyerMoore") : 0) << " ";
        out << (timesMap.count("KakyoinRabinKarp") ? timesMap.at("KakyoinRabinKarp") : 0) << " ";
        out << (timesMap.count("SpeedwagonBitap") ? timesMap.at("SpeedwagonBitap") : 0);
        out << "\n";
    }
    out.close();
    std::cout << "Created plot_data.txt\n";


    std::ofstream outAlgo("plot_data_algo.txt");
    if (!outAlgo.is_open()) {
        std::cerr << "Error: Cannot create plot_data_algo.txt\n";
        return;
    }
    
    outAlgo << "Algorithm AverageTime\n";

    for (const auto& [algo, timesVec] : timesPerAlgorithm) {
        double sum = 0;
        for (auto t : timesVec) sum += t;
        double avg = (timesVec.empty()) ? 0 : sum / timesVec.size();
        outAlgo << "\"" << algoDisplayName(algo) << "\" " << avg << "\n";
    }
    outAlgo.close();
    std::cout << "Created plot_data_algo.txt\n";

    std::ofstream outFile("plot_data_file.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot create plot_data_file.txt\n";
        return;
    }
    
    outFile << "File KnuthMorrisPratt BoyerMoore RabinKarp Bitap\n";

    for (const auto& [file, algoTimesMap] : timesPerFileAndAlgorithm) {
        outFile << "\"" << trimFilename(file) << "\" ";


        for (const std::string& algo : { "JotaroKMP", "DioBoyerMoore", "KakyoinRabinKarp", "SpeedwagonBitap" }) {
            if (algoTimesMap.count(algo) && !algoTimesMap.at(algo).empty()) {
                double sum = 0;
                for (auto t : algoTimesMap.at(algo)) sum += t;
                double avg = sum / algoTimesMap.at(algo).size();
                outFile << avg << " ";
            }
            else {
                outFile << "0 ";
            }
        }
        outFile << "\n";
    }
    outFile.close();
    std::cout << "Created plot_data_file.txt\n";
}

void writeGnuplotScript() {
    std::ofstream script("plot_script.plt");
    if (!script.is_open()) {
        std::cerr << "Error: Cannot create plot_script.plt\n";
        return;
    }
    
    script <<
        "set terminal pngcairo size 1400,700 enhanced font 'Verdana,10'\n" 
        "set output 'performance_chart.png'\n"
        "set title 'Algorithm Time Performance per pattern'\n"
        "set xlabel 'Source file (pattern)'\n"
        "set ylabel 'Time (ms)'\n"
        "set style data histogram\n"
        "set style histogram cluster gap 1\n"
        "set style fill solid border -1\n"
        "set boxwidth 0.9\n"
        "set xtics rotate by -45 scale 0\n"
        "set key outside right top vertical\n"
        "plot 'plot_data.txt' using 2:xtic(1) title 'Knuth-Morris-Pratt', \\\n"
        "     '' using 3 title 'Boyer-Moore', \\\n"
        "     '' using 4 title 'Rabin-Karp', \\\n"
        "     '' using 5 title 'Bitap'\n";
    script.close();
    std::cout << "Created plot_script.plt\n";
}


void writeGnuplotScriptAlgo() {
    std::ofstream script("plot_script_algo.plt");
    if (!script.is_open()) {
        std::cerr << "Error: Cannot create plot_script_algo.plt\n";
        return;
    }
    
    script <<
        "set terminal pngcairo size 1000,600 enhanced font 'Verdana,10'\n"
        "set output 'performance_chart_algo.png'\n"
        "set title 'Average Time per Algorithm'\n"
        "set xlabel 'Algorithm'\n"
        "set ylabel 'Average Time (ms)'\n"
        "set style data histogram\n"
        "set style fill solid border -1\n"
        "set boxwidth 0.7\n"
        "set xtics rotate by -45 scale 0\n"
        "plot 'plot_data_algo.txt' using 2:xtic(1) title 'Average Time'\n";
    script.close();
    std::cout << "Created plot_script_algo.plt\n";
}


void writeGnuplotScriptFile() {
    std::ofstream script("plot_script_file.plt");
    if (!script.is_open()) {
        std::cerr << "Error: Cannot create plot_script_file.plt\n";
        return;
    }
    
    script <<
        "set terminal pngcairo size 1400,700 enhanced font 'Verdana,10'\n"
        "set output 'performance_chart_file.png'\n"
        "set title 'Average Time per File and Algorithm'\n"
        "set xlabel 'Source file'\n"
        "set ylabel 'Average Time (ms)'\n"
        "set style data histogram\n"
        "set style histogram cluster gap 1\n"
        "set style fill solid border -1\n"
        "set boxwidth 0.9\n"
        "set xtics rotate by -45 scale 0\n"
        "set key outside right top vertical\n"
        "plot 'plot_data_file.txt' using 2:xtic(1) title 'Knuth-Morris-Pratt', \\\n"
        "     '' using 3 title 'Boyer-Moore', \\\n"
        "     '' using 4 title 'Rabin-Karp', \\\n"
        "     '' using 5 title 'Bitap'\n";
    script.close();
    std::cout << "Created plot_script_file.plt\n";
}


bool runGnuplotScript(const std::string& scriptName, const std::string& expectedOutput) {
    std::string command = "gnuplot " + scriptName;
    std::cout << "📊 Executing: " << command << " -> " << expectedOutput << "\n";
    
    int result = system(command.c_str());
    if (result == 0) {

        std::ifstream check(expectedOutput);
        if (check.good()) {
            std::cout << "✅ Successfully created " << expectedOutput << "\n";
            check.close();
            return true;
        } else {
            std::cerr << "❌ Script executed but " << expectedOutput << " not found\n";
            return false;
        }
    } else {
        std::cerr << "❌ Error executing " << scriptName << " (exit code: " << result << ")\n";
        std::cerr << "Make sure gnuplot is installed and in your PATH\n";
        return false;
    }
}

bool checkGnuplotInstallation() {
    std::cout << "🔍 Checking gnuplot installation...\n";
    

    int result1 = system("gnuplot --version >nul 2>&1");
    int result2 = system("gnuplot.exe --version >nul 2>&1");
    
    if (result1 == 0 || result2 == 0) {
        std::cout << "✅ Gnuplot is available\n";
        return true;
    } else {
        std::cerr << "⚠️  Warning: Gnuplot not found in PATH\n";
        std::cerr << "Please install gnuplot or add it to your PATH to generate charts\n";
        std::cerr << "You can download it from: http://www.gnuplot.info/\n";
        return false;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    JojoChrono timeSpirit;

    using SourceEntry = std::tuple<std::string, std::string, std::vector<std::string>>;
    std::vector<SourceEntry> sources;

    std::vector<std::string> filenames = {
        "Les Miserable.txt", "logs.txt", "dna.txt", "social.txt"
    };


    std::ofstream clearData("plot_data.txt", std::ios::trunc);
    clearData.close();

    for (const auto& filename : filenames) {
        std::string text = loadFile(filename);
        if (!text.empty()) {
            auto topWords = extractTopNWords(text, 5);
            sources.emplace_back(filename, text, topWords);
        }
        else {
            std::cerr << "Ошибка загрузки " << filename << "\n";
        }
    }

    const size_t numThreads = std::thread::hardware_concurrency();
    const size_t overlap = 64;

    std::cout << "[Info] Running tests with multithreading and summary...\n";

    for (const auto& [file, textData, patterns] : sources) {
        std::cout << "\n=== Testing file: " << trimFilename(file) << " (" << textData.size() << " chars) ===\n";

        for (const auto& pattern : patterns) {
            headerPrinted = false;
            auto parts = splitTextWithOverlap(textData, numThreads, overlap);
            std::map<std::string, AlgoResult> aggregatedResults;

            std::vector<std::string> algorithms = { "JotaroKMP", "DioBoyerMoore", "KakyoinRabinKarp" };
            if (pattern.size() <= 64) algorithms.push_back("SpeedwagonBitap");

            for (const auto& algo : algorithms) {
                std::vector<std::thread> threads;
                std::vector<AlgoResult> threadResults(parts.size());

                for (size_t i = 0; i < parts.size(); ++i) {
                    threads.emplace_back(threadSearchWorker, algo, parts[i], pattern, file, std::ref(timeSpirit), std::ref(threadResults[i]));
                }
                for (auto& t : threads) t.join();

                AlgoResult aggRes = {};
                int totalComparisons = 0;
                size_t totalMatches = 0;
                double totalTime = 0;

                for (const auto& tr : threadResults) {
                    totalComparisons += tr.comparisons;
                    totalMatches += tr.matches;
                    totalTime += tr.timeTaken;
                }
                aggRes.comparisons = totalComparisons;
                aggRes.matches = totalMatches;
                aggRes.timeTaken = totalTime;
                aggregatedResults[algo] = aggRes;


                std::string label = trimFilename(file) + " (" + pattern + ")";
                allResultsForPlot[label][algo] = totalTime;

                timesPerAlgorithm[algo].push_back(totalTime);

                timesPerFileAndAlgorithm[file][algo].push_back(totalTime);
            }

            compareAlgorithmsSummary(file, pattern, (int)textData.size(), aggregatedResults);
        }
    }

    std::cout << "\n=== Generating data files and plots ===\n";
    

    writeAllDataForPlot();
    

    writeGnuplotScript();
    writeGnuplotScriptAlgo();
    writeGnuplotScriptFile();


    if (checkGnuplotInstallation()) {
        std::cout << "\ngraphs\n";
        
        bool success1 = runGnuplotScript("plot_script.plt", "performance_chart.png");
        bool success2 = runGnuplotScript("plot_script_algo.plt", "performance_chart_algo.png");
        bool success3 = runGnuplotScript("plot_script_file.plt", "performance_chart_file.png");
        
        std::cout << "\nresults\n";
        int successCount = 0;
        if (success1) { 
            std::cout << "✅ performance_chart.png\n"; 
            successCount++;
        }
        if (success2) { 
            std::cout << "✅ performance_chart_algo.png\n"; 
            successCount++;
        }
        if (success3) { 
            std::cout << "✅ performance_chart_file.png м\n"; 
            successCount++;
        }
        
        if (successCount > 0) {

        } else {
            std::cout << "   gnuplot plot_script.plt\n";
            std::cout << "   gnuplot plot_script_algo.plt\n";
            std::cout << "   gnuplot plot_script_file.plt\n";
        }
    } else {
        std::cout << "   gnuplot plot_script.plt\n";
        std::cout << "   gnuplot plot_script_algo.plt\n";
        std::cout << "   gnuplot plot_script_file.plt\n";
        std::cout << "\n https://sourceforge.net/projects/gnuplot/\n";
    }

    return 0;
}