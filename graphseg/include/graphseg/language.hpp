#ifndef GRAPHSEG_CPP_GRAPHSEG_LANG_HPP
#define GRAPHSEG_CPP_GRAPHSEG_LANG_HPP

#include "graphseg/internal/utils/exec.hpp"
#include "graphseg/internal/utils/mecab_helper.hpp"

#include <filesystem>
#include <functional>
#include <iostream>
#include <mecab.h>
#include <string>
#include <type_traits>

namespace GraphSeg {
enum class Lang { EN, JP };

template <Lang LangType = Lang::EN> class Language {
protected:
  static std::string Locale() {
    if constexpr (LangType == Lang::JP) {
      return "ja_JP.UTF-8";
    }
  }

  static const std::string SentenceTagger(const std::string &s) {
    if constexpr (LangType == Lang::JP) {
      const auto tagger = MeCab::createTagger("");
      auto parsed_sentence = tagger->parse(s.c_str());
      return GraphSeg::internal::utils::ExtractTerm(parsed_sentence);
    } else {
      return s;
    }
  }
};

template <Lang LangType = Lang::EN> class Executable {
  template <Lang T> static constexpr auto false_v = false;
  static_assert(false_v<LangType>,
                "Specified ArticleProcessor is not implemented");
};

template <>
class Executable<Lang::JP> // I think that template specialization is better
                           // than if constexpr
{
protected:
  std::string Execute(const std::string &script, const std::string &input) {
    if (!ScriptExistence(ScriptPathExtractor(), script)) {
      std::runtime_error(ScriptPathExtractor() + "/" + script +
                         " doe's not exist");
    }

    int code;
    const std::string cmd =
        "echo " + input + " | " + CommandBaseExtractor() + "/" + script;
    auto result = internal::utils::exec(cmd.c_str(), code);
    return result;
  }

private:
  std::string CommandBaseExtractor() {
    return PythonPathExtractor() + " " + ScriptPathExtractor();
  }

  std::string ScriptPathExtractor() {
    const std::string scriptPath = getenv("PY_SCRIPT_PATH");
    assert(scriptPath.size() != 0);
    return scriptPath + "/jp";
  }

  std::string PythonPathExtractor() {
    const std::string pythonPath = getenv("PYTHON_PATH");
    assert(pythonPath.size() != 0);
    return pythonPath;
  }

  bool ScriptExistence(const std::string &path, const std::string &script) {
    for (const auto &x : std::filesystem::directory_iterator(path)) {
      if (path + "/" + script == x.path()) {
        return true;
      }
    }
    return false;
  }
};

template <> class Executable<Lang::EN> {
protected:
  std::string Execute(const std::string &script, const std::string &input) {
    if (!ScriptExistence(ScriptPathExtractor(), script)) {
      std::runtime_error(ScriptPathExtractor() + "/" + script +
                         " doe's not exist");
    }

    int code;
    const std::string cmd =
        "echo " + input + " | " + CommandBaseExtractor() + "/" + script;
    auto result = internal::utils::exec(cmd.c_str(), code);
    return result;
  }

private:
  std::string CommandBaseExtractor() {
    return PythonPathExtractor() + " " + ScriptPathExtractor();
  }

  std::string ScriptPathExtractor() {
    const std::string scriptPath = getenv("PY_SCRIPT_PATH");
    assert(scriptPath.size() != 0);
    return scriptPath + "/en";
  }

  std::string PythonPathExtractor() {
    const std::string pythonPath = getenv("PYTHON_PATH");
    assert(pythonPath.size() != 0);
    return pythonPath;
  }

  bool ScriptExistence(const std::string &path, const std::string &script) {
    for (const auto &x : std::filesystem::directory_iterator(path)) {
      if (path + "/" + script == x.path()) {
        return true;
      }
    }
    return false;
  }
};
} // namespace GraphSeg

#endif
