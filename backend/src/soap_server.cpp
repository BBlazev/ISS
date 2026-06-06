#include <cctype>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "soap_server.hpp"

#include "FileSearch.nsmap"
#include "file_entity.hpp"
#include "pugixml.hpp"
#include "soapFileSearchService.h"
#include "soapH.h"

static std::string g_xml_path;

std::vector<FileEntity> search_file(const std::string &xml_path, const std::string &term) {
    std::vector<FileEntity> out;

    std::string t;
    for (unsigned char x : term)
        t += std::tolower(x);

    std::replace(t.begin(), t.end(), '\'', '_');

    pugi::xml_document doc;
    if (!doc.load_file(xml_path.c_str()))
        throw std::runtime_error("Cannot parse XML: " + xml_path);

    const std::string LC = "'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'";

    std::string q = "/files/file["
                    "contains(translate(title," +
                    LC + "),'" + t +
                    "')"
                    " or contains(translate(description," +
                    LC + "),'" + t +
                    "')"
                    " or contains(translate(filename_download," +
                    LC + "),'" + t +
                    "')"
                    " or tags/tag[contains(translate(.," +
                    LC + "),'" + t +
                    "')]"
                    "]";

    for (pugi::xpath_node xn : doc.select_nodes(q.c_str())) {
        pugi::xml_node file = xn.node();

        FileEntity f;
        f.id = file.child_value("id");
        f.filename_download = file.child_value("filename_download");
        f.title = file.child_value("title");
        f.type = file.child_value("type");
        f.uploaded_on = file.child_value("uploaded_on");

        std::string fs = file.child_value("filesize");
        if (!fs.empty())
            f.filesize = std::stoll(fs);
        std::string w = file.child_value("width");
        if (!w.empty())
            f.width = std::stoi(w);
        std::string h = file.child_value("height");
        if (!h.empty())
            f.height = std::stoi(h);
        std::string d = file.child_value("description");
        if (!d.empty())
            f.description = d;

        for (pugi::xml_node tag : file.child("tags").children("tag"))
            f.tags.emplace_back(tag.child_value());

        out.push_back(std::move(f));
    }

    return out;
}

int FileSearchService::searchFiles(const std::string &term, std::vector<fs__FileResult> &result) {

    auto file = search_file(g_xml_path, term);

    for (const auto &f : file) {
        fs__FileResult r;
        r.id = f.id;
        r.filename_download = f.filename_download;
        r.title = f.title;
        r.type = f.type;
        r.filesize = f.filesize;
        r.description = f.description.value_or("");
        r.uploaded_on = f.uploaded_on;
        result.push_back(std::move(r));
    }
    return SOAP_OK;
}

int FileSearchService::hello(std::string &result) {
    result.append("GREETINGS");
    return SOAP_OK;
}

SoapServer::SoapServer(std::string xml_path, int port) : port_(port), xml_path_(xml_path) {}
SoapServer::~SoapServer() { stop(); }

void SoapServer::start() {
    g_xml_path = xml_path_;
    running_ = true;
    thread_ = std::thread(&SoapServer::run_loop, this);
}
void SoapServer::stop() {
    running_ = false;
    if (thread_.joinable())
        thread_.join();
}

void SoapServer::run_loop() {
    FileSearchService service;
    if (!soap_valid_socket(service.bind(nullptr, port_, 100))) {
        std::cerr << "SOAP: bind failed on port " << port_ << "\n";
        service.soap_stream_fault(std::cerr);
        return;
    }
    std::cout << "SOAP: listening on localhost:" << port_ << "\n";
    while (running_) {
        if (!soap_valid_socket(service.accept())) {
            if (service.soap->error)
                service.soap_stream_fault(std::cerr);
            continue;
        }
        if (service.serve() != SOAP_OK)
            service.soap_stream_fault(std::cerr);
        service.destroy();
    }
}
