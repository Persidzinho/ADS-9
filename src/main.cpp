// Copyright 2022 NNTU-CS
#include "tree.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

static std::vector<char> makeSymbolSet(int n) {
    std::vector<char> sym;
    for (int i = 1; i <= n; ++i) {
        sym.push_back(static_cast<char>('0' + i));
    }
    return sym;
}

template<typename Func>
double measureTime(Func&& func, int numCalls) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numCalls; ++i) {
        func();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count() / numCalls;
}

static int randomPermNumber(int total, std::mt19937& gen) {
    std::uniform_int_distribution<int> dist(1, total);
    return dist(gen);
}

static void savePlot(const std::vector<int>& sizes,
                     const std::vector<double>& time1,
                     const std::vector<double>& time2) {
    std::system("mkdir -p result");

    std::ofstream csv("result/times.csv");
    csv << "n,getPerm1_sec,getPerm2_sec\n";
    for (size_t i = 0; i < sizes.size(); ++i) {
        csv << sizes[i] << "," << time1[i] << "," << time2[i] << "\n";
    }
    csv.close();

    std::ofstream gp("result/plot.gnu");
    gp << "set terminal png size 800,600\n";
    gp << "set output 'result/plot.png'\n";
    gp << "set logscale y\n";
    gp << "set xlabel 'n (количество символов)'\n";
    gp << "set ylabel 'время (секунды)'\n";
    gp << "set title 'Сравнение getPerm1 и getPerm2'\n";
    gp << "set grid\n";
    gp << "plot 'result/times.csv' using 1:2 with linespoints title 'getPerm1', \\\n";
    gp << "     'result/times.csv' using 1:3 with linespoints title 'getPerm2'\n";
    gp.close();

    int ret = std::system("gnuplot result/plot.gnu");
    if (ret != 0) {
        std::cerr << "Warning: gnuplot not executed (maybe not installed)\n";
    } else {
        std::cout << "График сохранён: result/plot.png\n";
    }
}

int main() {
    const int maxN = 9;
    const int repeats = 10;
    std::vector<int> sizes;
    std::vector<double> timePerm1, timePerm2;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::cout << "Запуск вычислительного эксперимента (n от 1 до " << maxN << ")\n";
    std::cout << "Для каждого n выполняется " << repeats << " случайных запросов\n\n";

    for (int n = 1; n <= maxN; ++n) {
        std::vector<char> symbols = makeSymbolSet(n);
        PMTree tree(symbols);
        int total = tree.totalPerms();
        std::cout << "n = " << n << ", всего перестановок: " << total << std::endl;

        std::vector<int> permNumbers;
        for (int r = 0; r < repeats; ++r) {
            permNumbers.push_back(randomPermNumber(total, gen));
        }

        double avg1 = 0.0;
        for (int num : permNumbers) {
            auto func = [&]() { getPerm1(tree, num); };
            avg1 += measureTime(func, 1);
        }
        avg1 /= repeats;

        double avg2 = 0.0;
        for (int num : permNumbers) {
            auto func = [&]() { getPerm2(tree, num); };
            avg2 += measureTime(func, 1);
        }
        avg2 /= repeats;

        sizes.push_back(n);
        timePerm1.push_back(avg1);
        timePerm2.push_back(avg2);

        std::cout << "  getPerm1 среднее: " << avg1 << " сек\n";
        std::cout << "  getPerm2 среднее: " << avg2 << " сек\n\n";
    }

    savePlot(sizes, timePerm1, timePerm2);

    std::cout << "=== Проверка работы функций на примере {'1','2','3'} ===\n";
    std::vector<char> in = {'1','2','3'};
    PMTree demoTree(in);
    auto all = getAllPerms(demoTree);
    std::cout << "Все перестановки (getAllPerms):\n";
    for (const auto& perm : all) {
        for (char c : perm) std::cout << c;
        std::cout << " ";
    }
    std::cout << "\n\n";

    auto p1 = getPerm1(demoTree, 2);
    auto p2 = getPerm2(demoTree, 2);
    std::cout << "getPerm1(2) = ";
    for (char c : p1) std::cout << c;
    std::cout << "\ngetPerm2(2) = ";
    for (char c : p2) std::cout << c;
    std::cout << std::endl;

    return 0;
}
