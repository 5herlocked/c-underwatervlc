//
// Created by camv7 on 29/06/2021.
//

// Borrowed and modified from
// https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
// modified to be templated and up to spec on clang tidy problems


#ifndef BER_TOOL_CSVROW_H
#define BER_TOOL_CSVROW_H

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

template <class T>
class CSVRow {
public:
    std::string_view operator[](std::size_t index) const {
        return std::string_view(&m_line[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));
    }

    [[nodiscard]] std::size_t size() const {
        return m_data.size() - 1;
    }

    void readNextRow(std::istream &str) {
        std::getline(str, m_line);

        m_data.clear();
        m_data.emplace_back(-1);
        std::string::size_type pos = 0;
        while ((pos = m_line.find(',', pos)) != std::string::npos) {
            m_data.emplace_back(pos);
            ++pos;
        }
        // This checks for a trailing comma with no data after it.
        pos = m_line.size();
        m_data.emplace_back(pos);
    }

private:
    std::string m_line;
    std::vector<T> m_data;
};

template <class T>
std::istream &operator>>(std::istream &str, CSVRow<T> &data) {
    data.readNextRow(str);
    return str;
}

template <class T>
class CSVIterator {
public:
    typedef std::input_iterator_tag iterator_category;
    typedef CSVRow<T> value_type;
    typedef std::size_t difference_type;
    typedef CSVRow<T> *pointer;
    typedef CSVRow<T> &reference;

    explicit CSVIterator(std::istream &str) : m_str(str.good() ? &str : nullptr) { ++(*this); }

    CSVIterator() : m_str(nullptr) {}

    // Pre Increment
    CSVIterator &operator++() {
        if (m_str) { if (!((*m_str) >> m_row)) { m_str = nullptr; }}
        return *this;
    }

    // Post increment
    CSVIterator operator++(int) {
        CSVIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    CSVIterator operator+=(int rhs) {
        CSVIterator tmp(*this);
        (*this) += rhs;
        return tmp;
    }

    CSVRow<T> const &operator*() const { return m_row; }

    CSVRow<T> const *operator->() const { return &m_row; }

    bool operator==(CSVIterator const &rhs) {
        return ((this == &rhs) || ((this->m_str == nullptr) && (rhs.m_str == nullptr)));
    }

    bool operator!=(CSVIterator const &rhs) { return (*this) != rhs; }

private:
    std::istream *m_str;
    CSVRow<T> m_row;
};

template <class T>
class CSVRange
{
    std::istream&   stream;
public:
    explicit CSVRange(std::istream& str)
            : stream(str)
    {}
    [[nodiscard]] CSVIterator<T> begin() const {return CSVIterator<T>{stream};}
    [[nodiscard]] CSVIterator<T> end()   const {return CSVIterator<T>{};}
};

#endif //BER_TOOL_CSVROW_H
