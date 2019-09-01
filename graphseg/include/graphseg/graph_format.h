#ifndef GRAPHSEG_GRAPH_FORMAT_H
#define GRAPHSEG_GRAPH_FORMAT_H

namespace GraphSeg
{
  enum class GraphType
  {
    DIRECTED_GRAPH,
    UNDIRECTED_GRAPH
  };

  struct FormattedGraph
  {
    struct FormattedNode
    {
      struct FormattedTargetNode 
      {
        string id;
        double edge_weight;

        FormattedTargetNode(string _id, double _weight) : id(_id), edge_weight(_weight)
        {}
      };

      double score;
      vector<FormattedTargetNode> targets;
    };

    GraphType graph_type;
    unordered_map<string, FormattedNode> value;
  };

  FormattedGraph format(const SegmentGraph& sg)
  {
    using FormattedNode = FormattedGraph::FormattedNode;
    using FormattedTargetNode = FormattedNode::FormattedTargetNode;

    FormattedGraph fg;
    fg.graph_type = GraphType::UNDIRECTED_GRAPH;
    for (size_t i = 0; i < sg.GetGraphSize(); ++i)
    {
      const auto& adjacent_nodes = sg[i];
      FormattedNode fn;
      fn.score = 0; // この文脈ではノードの重みは必要ない
      for (const auto& adjacent_node: adjacent_nodes)
      {
        fn.targets.emplace_back(FormattedTargetNode(std::to_string(adjacent_node.first), adjacent_node.second));
      }
      fg.value[std::to_string(i)] = fn;
    }
    return fg;
  }

  template <typename CharT, typename Traits>
  basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const FormattedGraph& fg)
  {
    os << "{\n";
    os << "  \"graph_type\": " << NAMEOF_ENUM(fg.graph_type) << ",\n";
    os << "  \"value\": [\n";
    for (const auto& node: fg.value)
    {
      const auto& formatted_node = node.second;
      os << "    {\n";
      os << "      \"score\": " << formatted_node.score << ",\n";
      os << "      \"targets: [\"\n";
      for (const auto& target: formatted_node.targets)
      {
        os << "         {\n";
        os << "           \"id\"" << target.id << "\n";
        os << "           \"weight\"" << target.edge_weight << "\n"; 
        os << "         },\n";
      }
      os << "      ]\n";
      os << "    },\n";
    }
    os << "  ]\n";
    os << "}\n";
    return os;
  }

  template <typename CharT, typename Traits>
  basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const SegmentGraph& sg)
  {
    for(size_t i = 0; i < sg.GetGraphSize(); ++i)
    {
      os << i << ": " << sg.GetSentence(i).GetText() << " => " << "{";
      for(const auto& p: sg[i])
      {
        os << "{" << p.first << ", " << p.second << "},";
      }
      os << "}" << std::endl;
    }
    return os;
  }
}

#endif