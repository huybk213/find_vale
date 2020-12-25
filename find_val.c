#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

static char *input_file_name;
static char *output_file_name;

static bool file_exist(char * file_name)
{
    if (access(file_name, F_OK) == 0 )
    {
        // printf("Found\r\n");
        return true;
    } 
    else 
    {
        // file doesn't exist
        printf("File %s not found\r\n", file_name);
        return false;
    } 
}

void main(int argc, char * argv[])
{
    /**
     * input name, output file name, text to find format "begin,end"
     * "test.txt", "output.txt", "1234,123"
     */
    
    // printf("argc %d\r\n", argc);
    if (argc != 7)
    {
        printf("Invalid arguments\r\n");
        printf("Use command\r\n./findval -i input.txt -o output.txt -v \"pattern_begin,pattern_end\"\r\n");
        return;
    }
    int found_i = 0, found_o = 0, found_v = 0, found_begin_end = 0;
    int input_idx = 0, output_idx = 0, pattern_idx = 0;
    FILE * f_input = NULL;
    FILE * f_output = NULL;
    char * input_file_buffer = NULL;

    for (uint32_t i = 1; i < argc; i++)
    {
        // printf("argv \"%s\"\r\n", argv[i]);
        if (strstr(argv[i], "-i"))
        {
            found_i++;
            input_idx = i + 1;
        }
        else if (strstr(argv[i], "-o"))
        {
            found_o++;
            output_idx = i + 1;
        }
        else if (strstr(argv[i], "-v"))
        {
            found_begin_end++;
            pattern_idx = i + 1;
        }
        else
        {
            // printf("Invalid argv \"%s\"\r\n", argv[i]);
            // return;
        }
    }

    if (found_i != 1 || found_o != 1 || found_begin_end != 1)
    {
        printf("Invalid argv [i-o-v]-[%d-%d-%d]\r\n", found_i, found_o, found_v);
        return;
    }

    printf("Input file %s\r\nOutput file %s\r\nPattern to find %s\r\n", argv[input_idx], argv[output_idx], argv[pattern_idx]);
    if (!file_exist(argv[input_idx]))
    {
        return;
    }

    input_file_name = argv[input_idx];
    output_file_name = argv[output_idx];

    int val_size = -1, begin_idx = -1, found_end_idx = -1;
    int val_max_len = strlen(argv[pattern_idx]);

    for (int i = 0; i < val_max_len; i++)
    {
        if (*(argv[pattern_idx] + i) == ',')
        {
            if ((*(argv[pattern_idx]) == ',') && i == 0)
            {

            }
            else
            {
                if (begin_idx == -1)
                {
                    begin_idx = i;
                    if (i != (val_max_len - 1))
                    {
                        found_end_idx = 0;
                    }
                    break;
                }
            }
        }
    }

    if (begin_idx == -1 || found_end_idx == -1)
    {
        printf("Invalid pattern\r\n");
        return;
    }
    
    char * pattern_begin_str = (char*)calloc(begin_idx + 1, 1);
    char * pattern_end_str = (char*)calloc(val_max_len - begin_idx, 1);
    // char * val_character = (char*)calloc(end_idx - begin_idx, 0);

    if (pattern_begin_str == NULL || pattern_end_str == NULL)
    {
        if (pattern_begin_str)
            free(pattern_begin_str);
        if (pattern_end_str)
            free(pattern_end_str);

        printf("Not enough memory\r\n");
        return;
    }

    printf("Pattern size %d-%d\r\n", begin_idx, val_max_len - begin_idx - 1);

    memcpy(pattern_begin_str, argv[pattern_idx], begin_idx);
    memcpy(pattern_end_str, argv[pattern_idx]+ begin_idx + 1, val_max_len - begin_idx - 1);
    // memcpy(val_character, argv[pattern_idx] + begin_idx + 1, end_idx - begin_idx - 1);
    printf("Find number between \"%s\" and \"%s\"\r\n", pattern_begin_str, pattern_end_str);


    int file_size;

    f_input = fopen(argv[input_idx], "rb");
    if (f_input == NULL)
    {
        printf("Cannot open file %s\r\n", argv[input_idx]);
        goto end;
    }
    fseek(f_input, 0L, SEEK_END);
    file_size = ftell(f_input);
    if (file_size == 0)
    {
        goto end;
    }
    fseek(f_input, 0L, SEEK_SET);

    input_file_buffer = (char*)calloc(file_size + 1, 1);
    if (input_file_buffer == NULL)
    {
        printf("Cannot malloc %d bytes\r\n", file_size);
        goto end;
    }
    else
    {
        printf("Input file size %d\r\n", file_size);
    }
    

    int result = fread(input_file_buffer, 1, file_size, f_input);
    if (result != file_size) 
    {
        // fputs ("Reading error\r\n", stderr);
        perror("Reading error : ");
    }

    char * tmp_p = input_file_buffer;
    int found_val_count = 0;

    f_output = fopen(output_file_name, "wb");
    if (f_output == NULL)
    {
        printf("Cannot create output file\r\n");
        goto end;
    }

    while(1)
    {
        char * tmp_pattern_begin = NULL;
        char * tmp_pattern_end = NULL;
        int val_size = 0;
        if ((tmp_pattern_begin = strstr(tmp_p, pattern_begin_str)) 
            && (tmp_pattern_end = strstr(tmp_p, pattern_end_str))
            && ((val_size = tmp_pattern_end - tmp_pattern_begin) > 0))
        {
            char * hold_buffer = (char*)calloc(val_size + 1, 1);
            memcpy(hold_buffer, &tmp_pattern_begin[strlen(pattern_begin_str)], val_size - strlen(pattern_begin_str));
            char * tmp_buffer = hold_buffer;
            char * the_last = hold_buffer;
            while(1)
            {
                char * temp = NULL;
                // printf("Find %s in %s\r\n", pattern_begin_str, tmp_buffer);
                if (temp = strstr(tmp_buffer, pattern_begin_str))
                {
                    tmp_buffer = temp + strlen(pattern_begin_str);
                    the_last = temp;
                }
                else
                {
                    the_last = tmp_buffer;
                    // printf("Err\r\n");
                    break;
                }
            }

            static int count = 0;

            printf("[%d] : %s\r\n", count++, the_last);
            fwrite(the_last, 1, strlen(the_last), f_output);
            fwrite("\r\n", 1, 2, f_output);
            free(hold_buffer);

            tmp_p = tmp_pattern_end + strlen(pattern_end_str);
            found_val_count++;
        }
        else
        {
            break;
        }
        
    }

end:
    printf("Done\r\n");

    if (pattern_begin_str)
        free(pattern_begin_str);
    if (pattern_end_str)
        free(pattern_end_str);

    if (f_input)
    {
        fclose(f_input);
    }

    if (f_output)
    {
        fclose(f_output);
    }

    if (input_file_buffer)
    {
        free(input_file_buffer);
    }

    printf("Count %d\r\n", found_val_count);
    return;
}

