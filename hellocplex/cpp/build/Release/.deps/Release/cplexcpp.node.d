cmd_Release/cplexcpp.node := c++ -bundle -undefined dynamic_lookup -Wl,-search_paths_first -mmacosx-version-min=10.7 -arch x86_64 -L./Release  -o Release/cplexcpp.node Release/obj.target/cplexcpp/cplexcpp.o Release/obj.target/cplexcpp/cplexcpp_node.o /Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/concert/lib/x86-64_osx/static_pic/libconcert.a /Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/cplex/lib/x86-64_osx/static_pic/libcplex.a /Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/cplex/lib/x86-64_osx/static_pic/libcplexdistmip.a /Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/cplex/lib/x86-64_osx/static_pic/libilocplex.a