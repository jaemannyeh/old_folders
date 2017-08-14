
#include "yawrz.h"

#include <stack>

#include <curl/curl.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>

// Device:2.10 Root Object definition
//      https://www.broadband-forum.org/cwmp/tr-181-2-10-0-full.xml
// FAPService:2.1 Femto Access Point Service Data Model
//      https://www.broadband-forum.org/cwmp/tr-196-2-1-0-full.xml
// StorageService:1.2
//      https://www.broadband-forum.org/cwmp/tr-140-1-2-0-full.xml
// STBService:1.4 Service Object definition
//      https://www.broadband-forum.org/cwmp/tr-135-1-4-0-full.xml
// VoiceService:2.0 Service Object definition
//      https://www.broadband-forum.org/cwmp/tr-104-2-0-0-full.xml
DEFINE_string(url,"https://www.broadband-forum.org/cwmp/tr-181-2-10-0-full.xml","URL");
DEFINE_bool(csv, false, "csv format");
DEFINE_bool(pb, false, "protocol buffer");

static size_t xml_parse_chunk_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    // size_t real_size = size * nmemb; // 1 * 16384 (16384(=0x4000) is max)
    // std::cout << size << ":" << nmemb << ":" << real_size << std::endl;
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)userdata;
    if (ctxt) {
        xmlParseChunk(ctxt,ptr,size*nmemb,0);
    }

    return (size*nmemb);
}

static size_t html_parse_chunk_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    // size_t real_size = size * nmemb; // 1 * 16384 (16384(=0x4000) is max)
    // std::cout << size << ":" << nmemb << ":" << real_size << std::endl;
    htmlParserCtxtPtr ctxt = (htmlParserCtxtPtr)userdata;
    if (ctxt) {
        htmlParseChunk(ctxt,ptr,size*nmemb,0);
    }

    return (size*nmemb);
}

static int import_xml_html_using_curl(xmlParserCtxtPtr ctxt,const char *URL) {
    // https://curl.haxx.se/libcurl/c/xmlstream.html
    // curl_global_init(CURL_GLOBAL_ALL^CURL_GLOBAL_SSL);
    curl_global_init(CURL_GLOBAL_ALL);
    
    CURL *handle = curl_easy_init();
    if (handle == NULL) {
        LOG(INFO) << __FUNCTION__;
    }

    curl_easy_setopt(handle,CURLOPT_URL,URL);

    if (strcmp(strrchr(URL, '.'),".xml")==0) {
        // for example, https://www.broadband-forum.org/cwmp/tr-181-2-10-0-full.xml
        curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,xml_parse_chunk_callback);        
    }
    else {
        curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,html_parse_chunk_callback);        
    }

    curl_easy_setopt(handle,CURLOPT_WRITEDATA,ctxt);
  
    CURLcode res = curl_easy_perform(handle);                   
    if(res != CURLE_OK) { // not CURL_OK
        LOG(INFO) << __FUNCTION__;
    }
  
    curl_easy_cleanup(handle);
    
    curl_global_cleanup();
    
    if (strcmp(strrchr(URL, '.'),".xml")==0) {
        xmlParseChunk(ctxt,NULL,0,1); // terminate:	last chunk indicator
    }
    else {
        htmlParseChunk(ctxt,NULL,0,1); // terminate: last chunk indicator    
    }

    return 0;
}

static void walk_tree_cwmp_parameters(xmlNode * the_node) {
    // CPE WAN Management Protocol
    // CPE Customer Premise Equipment 
    // https://www.broadband-forum.org/cwmp/tr-181-2-10-0-full.xml # TR-069 Device:2.10 Root Object definition
    // https://www.broadband-forum.org/cwmp/tr-196-2-1-0-full.xml # FAPService:2.1 Femto Access Point Service Data Model
    // https://www.broadband-forum.org/cwmp/tr-140-1-2-0-full.xml # TR-140 StorageService:1.2
    // https://www.broadband-forum.org/cwmp/tr-135-1-4-0-full.xml # STBService:1.4 Service Object definition
    // https://www.broadband-forum.org/cwmp/tr-104-2-0-0-full.xml # TR-069 VoiceService:2.0 Service Object definition
    for (xmlNode *node = the_node; node; node = node->next) {
        if (xmlStrcmp(node->name, (const xmlChar *)"description")==0) {
            xmlChar *description_content = xmlNodeGetContent(node);
            
           if (node->parent && xmlStrcmp(node->parent->name,(const xmlChar *)"parameter")==0) {
                xmlChar *parameter_name = xmlGetProp(node->parent,(const xmlChar *)"name");
                if (node->parent->parent && xmlStrcmp(node->parent->parent->name,(const xmlChar *)"object")==0) {
                    xmlChar *object_name = xmlGetProp(node->parent->parent,(const xmlChar *)"name");
                    if (object_name && parameter_name && description_content) {
                        // csv format: NAME,VALUE,STATIC,OPTIONAL,DESCRIPTION
                        // printf("%s%s,'0',0,0,'%s'\n",object_name,parameter_name,description_content);
                        printf("'%s%s','0',0,0,''\n",object_name,parameter_name);
                    }
                }   
            }
            else if (node->parent && xmlStrcmp(node->parent->name,(const xmlChar *)"object")==0) {
                xmlChar *object_name = xmlGetProp(node->parent,(const xmlChar *)"name");
                if (object_name && description_content) {
                    printf("'%s','0',0,0,''\n",object_name);                    
                }
            }
            
        }
        
        walk_tree_cwmp_parameters(node->children);
    } // for
}

static void walk_tree_cwmp_parameters_2(xmlNode *the_node) {
    static xmlNode *the_root = (the_root == NULL) ? the_node : the_root;

    static std::stack<std::string> object_name_stack;
    static std::stack<std::string> message_stack;
    static const char *tabs[] = { "","\t","\t\t","\t\t\t","\t\t\t\t","\t\t\t\t\t","\t\t\t\t\t\t","\t\t\t\t\t\t\t","\t\t\t\t\t\t\t\t","\t\t\t\t\t\t\t\t\t","\t\t\t\t\t\t\t\t\t\t","\t\t\t\t\t\t\t\t\t\t\t" };
    static int unique_tag_id[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    
    for (xmlNode *node = the_node; node; node = node->next) {
        
        char *message = NULL;
        const char *object_name = NULL; // xmlChar *object_name = NULL;

        if (xmlStrcmp(node->name, (const xmlChar *)"description")==0) {
            xmlChar *description_content = xmlNodeGetContent(node);
            
            if (node->parent && xmlStrcmp(node->parent->name,(const xmlChar *)"parameter")==0) {
                xmlChar *parameter_name = xmlGetProp(node->parent,(const xmlChar *)"name");
                if (node->parent->parent && xmlStrcmp(node->parent->parent->name,(const xmlChar *)"object")==0) {
                    object_name = (const char *)xmlGetProp(node->parent->parent,(const xmlChar *)"name");
                    if (object_name && parameter_name && description_content) {
                        // csv format: NAME,VALUE,STATIC,OPTIONAL,DESCRIPTION
                        // printf("%s%s,'0',0,0,'%s'\n",object_name,parameter_name,description_content);
                        // printf("'%s%s','0',0,0,''\n",object_name,parameter_name);
                        printf("%soptional string %s = %d;\n",tabs[object_name_stack.size()-1+1],parameter_name,unique_tag_id[object_name_stack.size()-1+1]+1);
                        unique_tag_id[object_name_stack.size()-1+1] += 1;                        
                    }
                }   
            }
            else if (node->parent && xmlStrcmp(node->parent->name,(const xmlChar *)"object")==0) {
                object_name = (const char *)xmlGetProp(node->parent,(const xmlChar *)"name");
                if (object_name && description_content) {
                    //printf("'%s','0',0,0,''\n",object_name);                    
                    
                    char object_path[PATH_MAX];
                    strcpy(object_path,(const char *)object_name);
                    
                    int len = strlen(object_path);
                    if (len > 5 && object_path[len-4] == '{') {
                        object_path[len-4] = 0;
                        len = strlen(object_path);
                    }
                    //printf("'%s','0',0,0,''\n",object_path);                    
                    
                    if (len > 1 && object_path[len-1] == '.') {
                        object_path[len-1] = 0;                    
                        if (strrchr(object_path,'.')) {
                            message = strrchr(object_path,'.')+1;
                        }
                        else {
                            message = object_path;
                        }

                        if (!object_name_stack.empty() && strstr(object_name,object_name_stack.top().c_str())==NULL) {
                            do {
                                unique_tag_id[object_name_stack.size()-1] += 1;
                                
                                printf("%s} // %lu // %s\n",tabs[object_name_stack.size()-1],object_name_stack.size()-1,object_name_stack.top().c_str());
                                //printf("%s} // %lu\n",tabs[object_name_stack.size()-1],object_name_stack.size()-1);

                                char name1[PATH_MAX]; strcpy(name1,message_stack.top().c_str()); name1[0] = toupper(name1[0]); // message name
                                char name2[PATH_MAX]; strcpy(name2,message_stack.top().c_str()); name2[0] = tolower(name2[0]); // field name

                                const char *dot = object_name_stack.top().c_str()+object_name_stack.top().length();                                
                                if (*(dot-2) == '}') {
                                    printf("%srepeated %s %s = %d; // %lu\n",tabs[object_name_stack.size()-1],name1,name2,unique_tag_id[object_name_stack.size()-1]+1,object_name_stack.size()-1);
                                }
                                else {
                                    printf("%soptional %s %s = %d; // %lu\n",tabs[object_name_stack.size()-1],name1,name2,unique_tag_id[object_name_stack.size()-1]+1,object_name_stack.size()-1);
                                }
                                
                                for (unsigned int u=(object_name_stack.size()-1+1); u<(sizeof(unique_tag_id)/sizeof(unique_tag_id[0])); u++) {
                                    unique_tag_id[u] = 0;
                                }

                                message_stack.pop();                                
                                object_name_stack.pop();                                
                            } while (!object_name_stack.empty() && strstr(object_name,object_name_stack.top().c_str())==NULL);
                        }
                        else {
                        }
                        
                        printf("%smessage %s { // %lu // %s\n",tabs[object_name_stack.size()],message,object_name_stack.size(),object_name);
                        //printf("%smessage %s {\n",tabs[object_name_stack.size()],message);
                        //printf("%s\trepeated Parameter parameter = 1;\n",tabs[object_name_stack.size()]);
                        message_stack.push(message);
                        object_name_stack.push(object_name);
                        
                    }
                }
            }
            
        }

        walk_tree_cwmp_parameters_2(node->children);

    } // for
    
    if (the_root == the_node) {
        while (object_name_stack.size()>1) {
            unique_tag_id[object_name_stack.size()-1] += 1;
                                            
            printf("%s} // %lu // %s\n",tabs[object_name_stack.size()-1],object_name_stack.size()-1,object_name_stack.top().c_str());

            char name1[PATH_MAX]; strcpy(name1,message_stack.top().c_str()); name1[0] = toupper(name1[0]); // message name
            char name2[PATH_MAX]; strcpy(name2,message_stack.top().c_str()); name2[0] = tolower(name2[0]); // field name
            
            const char *dot = object_name_stack.top().c_str()+object_name_stack.top().length();
            if (*(dot-2) == '}') {
                printf("%srepeated %s %s = %d; // %lu\n",tabs[object_name_stack.size()-1],name1,name2,unique_tag_id[object_name_stack.size()-1]+1,object_name_stack.size()-1);
            }
            else {
                printf("%soptional %s %s = %d; // %lu\n",tabs[object_name_stack.size()-1],name1,name2,unique_tag_id[object_name_stack.size()-1]+1,object_name_stack.size()-1);
            }
                            
            for (unsigned int u=(object_name_stack.size()-1+1); u<(sizeof(unique_tag_id)/sizeof(unique_tag_id[0])); u++) {
                unique_tag_id[u] = 0;
            }
                                
            message_stack.pop();
            object_name_stack.pop();
        } // while
        printf("} // %s\n",message_stack.top().c_str());
    }
}

static void walk_tree_html(xmlNode * the_node) {
    // Typedef xmlNodePtr htmlNodePtr
    // http://stackoverflow.com/questions/27215736/xml-parsing-with-libxml-in-c-is-not-showing-attribute-name
    for (xmlNode *node = the_node; node; node = node->next) {
        if (node->name) {
            printf("%s\n",node->name);
            for (xmlAttr *attr = node->properties; attr; attr = attr->next) {
                if (attr->name) {
                    printf("\t%s\n",attr->name);
                    // Attributes come in name/value pairs like: name="value"
                    xmlChar *attr_value = xmlNodeGetContent((xmlNode *)attr);
                    printf("\t\t%s\n",attr_value);
                }
            }
        }

        walk_tree_html(node->children);
    }
}

int import_xml_html_into_csv(const char *url) {
    if (url == NULL) {
        LOG(INFO) << "URL not defined";
        return -1;
    }
    
    if (strcmp(strrchr(url, '.'),".xml")==0) {
        // http://xmlsoft.org/examples/parse3.c
        // http://xmlsoft.org/examples/parse4.c
        // http://xmlsoft.org/tutorial/xmltutorial.pdf
        xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt(NULL,NULL,NULL,0,NULL);
        if (ctxt == NULL) {
            LOG(INFO) << __FUNCTION__;
        }
        
        import_xml_html_using_curl(ctxt,url);
    
        if (FLAGS_pb) {
            walk_tree_cwmp_parameters_2(xmlDocGetRootElement(ctxt->myDoc));
        }
        else if (FLAGS_csv) {
            walk_tree_cwmp_parameters(xmlDocGetRootElement(ctxt->myDoc));            
        }
        
        xmlFreeDoc(ctxt->myDoc);
    
        xmlFreeParserCtxt(ctxt);
        ctxt = NULL;
        
        xmlCleanupParser();
    }
    else {
        htmlParserCtxtPtr ctxt = htmlCreatePushParserCtxt(NULL, NULL, NULL, 0, NULL, XML_CHAR_ENCODING_NONE);
        if (ctxt == NULL) {
            LOG(INFO) << __FUNCTION__;
        }
    
        import_xml_html_using_curl(ctxt,url);
        
        walk_tree_html(xmlDocGetRootElement(ctxt->myDoc));
            
        xmlFreeDoc(ctxt->myDoc);
        
        htmlFreeParserCtxt(ctxt);
        ctxt = NULL;
    
        //htmlCleanupParser();
    }
    
    return 0;
}

namespace yawrz {
    
int tables_main(int argc,char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler(); // see glog/logging.h
    
    google::SetVersionString(yawrz::version());
    google::SetUsageMessage("");
    google::ParseCommandLineFlags(&argc, &argv, true); //  see logging_unittest.cc

    import_xml_html_into_csv( FLAGS_url.c_str() );

    // sudo apt-get install libprotobuf-dev protobuf-compiler
    //google::protobuf::ShutdownProtobufLibrary();
    
    return 0;
}

} // namespace yawarz
