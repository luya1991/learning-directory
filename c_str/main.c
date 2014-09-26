
#include"myheader.h"

int main()
{
    /*
    ngx_chain_t* chain;
    char* request_content=NULL;
    chain = r->request_body->bufs;
        for (; chain; chain = chain->next)
        {
            char* temp;
            temp = print_start2end(chain->buf->start,chain->buf->end);
            request_content = strcat(request_content, temp);
        }

    // send http header
    ngx_str_t type = ngx_string("text/plain");
    ngx_str_t response;
    response.data = (u_char*)request_content;
    response.len = strlen(request_content);

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = response.len;
    r->headers_out.content_type = type;
    */

    char* content = "ni hao";
    char* start = content;
    char* end = content + strlen(content);
    // c风格字符串头为双指针的习惯用法
    // head ----> 第一个字符的地址
    // tail ----> 最后一个字符的下一个地址，也就是'\0'

    printf("start: %c, end: %c\n",*start,*end);
    printf("content(%%s): %s, start(%%s): %s\n",content,start);
    //字符串变量指针和字符串head指针对于printf具有等价性

    char temp[1024];

    unsigned int size = end - start + 1;
    // '+1' 非常重要！！必须把end后面的'\0'拷过来！！

    printf("size: %u\n", size);
    memcpy(temp, start, size);
    printf("temp: %s\n",temp);
    return 0;
}
