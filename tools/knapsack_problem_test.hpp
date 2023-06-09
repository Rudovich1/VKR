#pragma once

// #define KNAPSACK_TEST

#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <climits>
#include <cfloat>
#include <algorithm>
#include <string>
#include <tuple>
#include <sstream>
#include <functional>

struct Statistics
{
    struct MetaData
    {
        using ReturnType = std::pair<double, std::string>;
        using Data = std::vector<ReturnType>;
        Data data_;

        Data::const_iterator begin() const
        {
            return data_.begin();
        }

        Data::const_iterator end() const
        {
            return data_.end();
        }

        MetaData& operator<< (ReturnType ret_type)
        {
            data_.emplace_back(ret_type);
            return *this;
        }

        MetaData& operator<< (std::pair<double, const char*> ret_type)
        {
            data_.emplace_back(ret_type);
            return *this;
        }

        MetaData& operator<< (const MetaData& meta_data)
        {
            for (auto& i: meta_data)
            {
                data_.emplace_back(i);
            }
            return *this;
        }

        const ReturnType& operator[] (size_t i) const
        {
            return data_[i];
        }

        ReturnType& operator[] (size_t i)
        {
            return data_[i];
        }

        size_t size() const
        {
            return data_.size();
        }

    };

    virtual void clear() = 0;
    virtual MetaData getMeta() const = 0;
};

struct DataStatistics: virtual public Statistics
{
    double min_result = LLONG_MAX;
    double max_result = LLONG_MIN;
    double average_result = 0.;

    virtual void clear() override
    {
        min_result = LLONG_MAX;
        max_result = LLONG_MIN;
        average_result = 0.;
    }

    virtual MetaData getMeta() const override
    {
        return MetaData() 
            << std::make_pair(min_result, "min_result") 
            << std::make_pair(max_result, "max_result")
            << std::make_pair(average_result, "average_result");
    }
};

struct TimeStatistics: virtual public Statistics
{
    double average_work_time = 0.;
    double min_work_time = LLONG_MAX;
    double max_work_time = LLONG_MIN;
    double total_work_time = 0.;

    virtual void clear()
    {
        average_work_time = 0.;
        min_work_time = LLONG_MAX;
        max_work_time = LLONG_MIN;
        total_work_time = 0.;
    }

    virtual MetaData getMeta() const override
    {
        return MetaData() 
            << std::make_pair(min_work_time, "min_work_time") 
            << std::make_pair(max_work_time, "max_work_time")
            << std::make_pair(average_work_time, "average_work_time")
            << std::make_pair(total_work_time, "total_work_time");
    }
};

struct BaseStatistics: public DataStatistics, public TimeStatistics
{
    std::string meta;

    virtual void clear() override
    {
        DataStatistics::clear();
        TimeStatistics::clear();
        meta.clear();
    }

    friend bool operator< (BaseStatistics& stat1, BaseStatistics& stat2)
    {
        return stat1.max_result < stat2.max_result;
    } 

    virtual MetaData getMeta() const override
    {
        return DataStatistics::getMeta() << TimeStatistics::getMeta();
    }
};

struct GenerationStatistics: public BaseStatistics 
{
    double num_chromosomes = 0.;

    void clear() override
    {
        BaseStatistics::clear();
        num_chromosomes = 0.;
    }

    MetaData getMeta() const override
    {
        return BaseStatistics::getMeta() 
            << std::make_pair(num_chromosomes, "num_chromosomes");
    }
};

struct PopulationStatistics: public BaseStatistics
{
    std::vector<GenerationStatistics> generations_statistics;
    double num_improve = 0.;

    void clear() override
    {
        BaseStatistics::clear();
        generations_statistics.clear();
        num_improve = 0.;
    }

    MetaData getMeta() const override
    {
        return BaseStatistics::getMeta() 
            << std::make_pair(num_improve, "num_improve")
            << std::make_pair((double)generations_statistics.size(), "number_generations");
    }
};

struct RunStatistic: public BaseStatistics
{
    std::vector<PopulationStatistics> populations_statistics;
    double min_num_improve = LLONG_MAX;
    double max_num_improve = LLONG_MIN;
    double average_num_improve = 0.;

    void clear() override
    {
        BaseStatistics::clear();
        populations_statistics.clear();
        min_num_improve = LLONG_MAX;
        max_num_improve = LLONG_MIN;
        average_num_improve = 0.;
    }

    MetaData getMeta() const override
    {
        return BaseStatistics::getMeta() 
            << std::make_pair((double)populations_statistics.size(), "num_populations")
            << std::make_pair(min_num_improve, "min_num_improve")
            << std::make_pair(max_num_improve, "max_num_improve")
            << std::make_pair(average_num_improve, "average_num_improve");
    }
};

struct GeneralStatistics: public BaseStatistics
{
    std::vector<RunStatistic> runs_statistics;
    double min_num_populations = LLONG_MAX;
    double max_num_populations = LLONG_MIN;
    double average_num_populations = 0.;

    void clear() override
    {
        BaseStatistics::clear();
        runs_statistics.clear();
        min_num_populations = LLONG_MAX;
        max_num_populations = LLONG_MIN;
        average_num_populations = 0.;
    }

    MetaData getMeta() const override
    {
        return BaseStatistics::getMeta() 
            << std::make_pair(min_num_populations, "min_num_populations")
            << std::make_pair(max_num_populations, "max_num_populations") 
            << std::make_pair(average_num_populations, "average_num_populations");
    }
};

class KnapsackProblemTest
{
    static inline GenerationStatistics generation_cache;
    static inline PopulationStatistics population_cache;
    static inline RunStatistic run_cache;
    static inline GeneralStatistics general_cache;
    static inline std::vector<GeneralStatistics> total_cache;

public:

    struct Log
    {
        static void inden(std::ostream& out, size_t size, char sep = '|')
        {
            while(size > 0)
            {
                out << sep << "       ";
                --size;
            }
        }

        static void logData(std::ostream& out, Statistics& stat, size_t inden_size)
        {
            int column_size = 27;

            std::stringstream names, sep, values;
            Statistics::MetaData meta_data = stat.getMeta();
            sep << "|";
            size_t size = 0;

            for(size_t i = 0; i < meta_data.size() - 1; ++i)
            {
                names << "| " << std::setw(column_size) << std::left << meta_data[i].second << " ";
                size += column_size + 3;
                values << "| " << std::setfill(' ') << std::setw(column_size) << std::left << std::fixed << meta_data[i].first << " ";
                sep << std::setfill('-') << std::setw(column_size + 3) << std::right << '+';
            }

            names << "| " << std::setw(column_size) << std::left << meta_data[meta_data.size() - 1].second << " |";
            size += column_size + 4;
            values << "| " << std::setfill(' ') << std::setw(column_size) << std::left << meta_data[meta_data.size() - 1].first << " |";
            sep << std::setfill('-') << std::setw(column_size + 3) << std::right << '|';

            inden(out, inden_size - 1);
            inden(out, 1, '#');
            out << std::setfill('=') << std::setw(size + 1) << std::right << " ";
            out << '\n';

            inden(out, inden_size - 1);
            inden(out, 1, '#');
            out << names.str() << '\n';

            inden(out, inden_size - 1);
            inden(out, 1, '#');
            out << sep.str() << '\n';
            
            inden(out, inden_size - 1);
            inden(out, 1, '#');
            out << values.str() << '\n';

            inden(out, inden_size - 1);
            inden(out, 1, '#');
            out << std::setfill('=') << std::setw(size + 1) << std::right << " ";
            out << '\n';
        }

        static void generationLog(std::ostream& out, GenerationStatistics& gen_stat, size_t num, size_t inden_size)
        {
            inden(out, inden_size);
            out << "Generation: " << std::setw(5) << std::setfill(' ') << std::left << std::to_string(num) + ": " << gen_stat.meta << '\n';

            inden(out, inden_size);
            out << '{' << '\n';

            logData(out, gen_stat, inden_size + 1);

            inden(out, inden_size);
            out << '}' << '\n';
        }

        static void populationLog(std::ostream& out, PopulationStatistics& population_stat, size_t num, size_t depth, size_t inden_size)
        {
            inden(out, inden_size);
            out << "Population: " << std::setw(5) << std::setfill(' ') << std::left << std::to_string(num) + ": " << population_stat.meta << '\n';
            
            inden(out, inden_size);
            out << '{' << '\n';

            if (depth > inden_size)
            {
                for (size_t i = 0; i < population_stat.generations_statistics.size(); ++i)
                {
                    generationLog(out, population_stat.generations_statistics[i], i + 1, inden_size + 1); 
                    inden(out, inden_size + 1);
                    out << '\n';
                }
            }
            
            logData(out, population_stat, inden_size + 1);

            inden(out, inden_size);
            out << '}' << '\n';
        }

        static void runLog(std::ostream& out, RunStatistic& run_stat, size_t num, size_t depth, size_t inden_size)
        {
            inden(out, inden_size);
            out << "Run: " << std::setw(5) << std::setfill(' ') << std::left << std::to_string(num) + ": " << run_stat.meta << '\n';
            
            inden(out, inden_size);
            out << '{' << '\n';

            if (depth > inden_size)
            {
                for (size_t i = 0; i < run_stat.populations_statistics.size(); ++i)
                {
                    populationLog(out, run_stat.populations_statistics[i], i + 1, depth, inden_size + 1); 
                    inden(out, inden_size + 1);
                    out << '\n';
                }
            }
            
            logData(out, run_stat, inden_size + 1);

            inden(out, inden_size);
            out << '}' << '\n';
        }

        static void generalLog(std::ostream& out, GeneralStatistics& general_stat, size_t num, size_t depth, size_t inden_size)
        {
            inden(out, inden_size);
            out << "General " << std::setw(5) << std::setfill(' ') << std::left << std::to_string(num) + ": " << general_stat.meta << '\n';

            inden(out, inden_size);
            out << '{' << '\n';

            if (depth > inden_size)
            {
                for (size_t i = 0; i < general_stat.runs_statistics.size(); ++i)
                {
                    runLog(out, general_stat.runs_statistics[i], i + 1, depth, inden_size + 1); 
                    inden(out, inden_size + 1);
                    out << '\n';
                }
            }

            logData(out, general_stat, inden_size + 1);

            inden(out, inden_size);
            out << '}' << '\n';
        }

        static void logDump(std::ostream& out, size_t depth, bool sort, std::string meta, size_t inden_size = 0)
        {
            if (sort)
            {
                std::sort(total_cache.begin(), total_cache.end());
            }

            inden(out, inden_size);
            out << "Total " << total_cache.size() << ": " << meta << '\n';
            inden(out, inden_size);
            out << '{' << '\n';

            for (size_t i = 0; i < total_cache.size(); ++i)
            {
                generalLog(out, total_cache[i], i + 1, depth, inden_size + 1);
                inden(out, inden_size + 1);
                out << '\n';
            }

            inden(out, inden_size);
            out << '}' << '\n';

            total_cache.clear();
        }
    };

    static void chromosomeDump(double fitness, std::string meta = "")
    {
        generation_cache.average_result += fitness;
        generation_cache.max_result = std::max(generation_cache.max_result, fitness);
        generation_cache.min_result = std::min(generation_cache.min_result, fitness);
        ++generation_cache.num_chromosomes;
    }

    static void generationDump(double work_time, std::string meta = "")
    {
        generation_cache.average_work_time = generation_cache.max_work_time = generation_cache.min_work_time = generation_cache.total_work_time = work_time;
        generation_cache.average_result /= (double) generation_cache.num_chromosomes;
        generation_cache.average_work_time /= (double) generation_cache.num_chromosomes;

        generation_cache.meta = meta;

        if (population_cache.max_result < generation_cache.max_result)
        {
            ++population_cache.num_improve;
        }

        population_cache.average_result += generation_cache.max_result;
        population_cache.average_work_time += generation_cache.total_work_time;
        population_cache.generations_statistics.emplace_back(generation_cache);
        population_cache.max_result = std::max(population_cache.max_result, generation_cache.max_result);
        population_cache.max_work_time = std::max(population_cache.max_work_time, generation_cache.total_work_time);
        population_cache.min_result = std::min(population_cache.min_result, generation_cache.max_result);
        population_cache.min_work_time = std::min(population_cache.min_work_time, generation_cache.total_work_time);
        population_cache.total_work_time += generation_cache.total_work_time;

        generation_cache.clear();
    }

    static void populationDump(std::string meta = "")
    {
        population_cache.average_result /= (double)population_cache.generations_statistics.size();
        population_cache.average_work_time /= (double)population_cache.generations_statistics.size();
        population_cache.meta = meta;

        run_cache.average_num_improve += population_cache.num_improve;
        run_cache.average_result += population_cache.max_result;
        run_cache.average_work_time += population_cache.total_work_time;
        run_cache.max_num_improve = std::max(run_cache.max_num_improve, population_cache.num_improve);
        run_cache.max_result = std::max(run_cache.max_result, population_cache.max_result);
        run_cache.max_work_time = std::max(run_cache.max_work_time, population_cache.total_work_time);
        run_cache.min_num_improve = std::min(run_cache.min_num_improve, population_cache.num_improve);
        run_cache.min_result = std::min(run_cache.min_result, population_cache.max_result);
        run_cache.min_work_time = std::min(run_cache.min_work_time, population_cache.total_work_time);
        run_cache.total_work_time += population_cache.total_work_time;
        run_cache.populations_statistics.emplace_back(population_cache);

        population_cache.clear();
    }

    static void runDump(std::string meta = "")
    {
        run_cache.average_result /= (double) run_cache.populations_statistics.size();
        run_cache.average_work_time /= (double) run_cache.populations_statistics.size();
        run_cache.average_num_improve /= (double) run_cache.populations_statistics.size();
        run_cache.meta = meta;

        general_cache.average_num_populations += run_cache.populations_statistics.size();
        general_cache.average_result += run_cache.max_result;
        general_cache.average_work_time += run_cache.total_work_time;
        general_cache.max_num_populations = std::max(general_cache.max_num_populations, (double)run_cache.populations_statistics.size());
        general_cache.max_result = std::max(general_cache.max_result, run_cache.max_result);
        general_cache.max_work_time = std::max(general_cache.max_work_time, run_cache.total_work_time);
        general_cache.min_num_populations = std::min(general_cache.min_num_populations, (double)run_cache.populations_statistics.size());
        general_cache.min_result = std::min(general_cache.min_result, run_cache.max_result);
        general_cache.min_work_time = std::min(general_cache.min_work_time, run_cache.total_work_time);
        general_cache.runs_statistics.emplace_back(run_cache);
        general_cache.total_work_time += run_cache.total_work_time;

        run_cache.clear();
    }

    static void generalDump(std::string meta = "")
    {
        general_cache.average_result /= (double) general_cache.runs_statistics.size();
        general_cache.average_work_time /= (double) general_cache.runs_statistics.size();
        general_cache.average_num_populations /= (double) general_cache.runs_statistics.size();
        general_cache.meta = meta;

        std::sort(general_cache.runs_statistics.begin(), general_cache.runs_statistics.end());

        total_cache.emplace_back(general_cache);

        general_cache.clear();
    }
};

#define _CHROMOSOME_DUMP_(fitness, meta) KnapsackProblemTest::chromosomeDump(fitness, meta)

#define _GENERATION_DUMP_(work_time, meta) KnapsackProblemTest::generationDump(work_time, meta)

#define _POPULATION_DUMP_(meta) KnapsackProblemTest::populationDump(meta)

#define _RUN_DUMP_(meta) KnapsackProblemTest::runDump(meta)

#define _GENERAL_DUMP_(meta) KnapsackProblemTest::generalDump(meta)

#define _LOG_DUMP_(out, depth, sort, meta) KnapsackProblemTest::Log::logDump(out, depth, sort, meta)