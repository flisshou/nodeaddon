{
  "targets": [
    {
      "target_name": "cplexcpp",
      "sources": ["cplexcpp.cc", "cplexcpp_node.cc"],
      "libraries": ["/Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/concert/lib/x86-64_osx/static_pic/libconcert.a",
                    "/Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/cplex/lib/x86-64_osx/static_pic/libcplex.a",
                    "/Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/cplex/lib/x86-64_osx/static_pic/libcplexdistmip.a",
                    "/Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/cplex/lib/x86-64_osx/static_pic/libilocplex.a"],
      "include_dirs": ["/Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/concert/include/",
                       "/Users/flisshou/Applications/IBM/ILOG/CPLEX_Studio_Community1263/cplex/include/"],
      "conditions": [["OS=='mac'",{"defines": ["__MACOSX_CORE__"],
                                   "xcode_settings": {"GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                                                      "OTHER_CFLAGS": ["-DIL_STD"],
                                                      "OTHER_CPLUPLUSFLAGS": ["-DIL_STD"],
                                                      "CLANG_CXX_LIBRARY": "libstdc++",
                                                      "CLANG_CXX_LANGUAGE_STANDARD": "c++11",
                                                      "MACOSX_DEPLOYMENT_TARGET": "10.7"}
                                   }
                    ]]
     }
  ]
}
