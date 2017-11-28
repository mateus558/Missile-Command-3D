#include "utils.h"
#include <GL/glut.h>

using namespace std;

namespace Drawing{
	void drawText(float x, float y, float z, float r, float g, float b, int font, const char *string){
		glColor3f( r, g, b );
		glRasterPos3f(x, y, z);
		int len, i;
		len = (int)strlen(string);

		for (i = 0; i < len; i++) {
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18 , string[i]);
		}
	}

	void drawEllipse(float x, float y, float z, float r, float g, float b, float xradius, float yradius, int npartes){
		glColor4f(r, g, b, 0.0f);
		glBegin(GL_POLYGON);
			for(int ii = 0; ii < npartes; ii++)
			{
				float theta = 2.0f * 3.1415926f * float(ii) / float(npartes);//get the current angle

				float xi = xradius * cosf(theta);//calculate the x component
				float yi = yradius * sinf(theta);//calculate the y component

				glVertex3f(xi + x, yi + y, z);//output vertex

			}
		glEnd();
	}
	
	GLvoid draw_circle(const GLfloat radius,const GLuint num_vertex, const GLfloat z)
	{
	  GLfloat vertex[4]; 
	  GLfloat texcoord[2];
	 
	  const GLfloat delta_angle = 2.0*M_PI/num_vertex;
	 
	  glBegin(GL_TRIANGLE_FAN);
	 
	  //draw the vertex at the center of the circle
	  texcoord[0] = 0.5;
	  texcoord[1] = 0.5;
	  glTexCoord2fv(texcoord);
	 
	  vertex[0] = vertex[1] = 0.0;
	  vertex[2] = z;
	  vertex[3] = 1.0;        
	  glVertex4fv(vertex);
	 
	  for(int i = 0; i < num_vertex ; i++)
	  {
		texcoord[0] = (std::cos(delta_angle*i) + 1.0)*0.5;
		texcoord[1] = (std::sin(delta_angle*i) + 1.0)*0.5;
		glTexCoord2fv(texcoord);
	 
		vertex[0] = std::cos(delta_angle*i) * radius;
		vertex[1] = std::sin(delta_angle*i) * radius;
		vertex[2] = z;
		vertex[3] = 1.0;
		glVertex4fv(vertex);
	  }
	 
	  texcoord[0] = (1.0 + 1.0)*0.5;
	  texcoord[1] = (0.0 + 1.0)*0.5;
	  glTexCoord2fv(texcoord);
	 
	  vertex[0] = 1.0 * radius;
	  vertex[1] = 0.0 * radius;
	  vertex[2] = z;
	  vertex[3] = 1.0;
	  glVertex4fv(vertex);
	  glEnd();
	 
	  //glDisable(GL_TEXTURE_2D);
	 
	}
}

GLuint png_texture_load(const char * file_name, int * width, int * height)
{
    // This function was originally written by David Grayson for
    // https://github.com/DavidEGrayson/ahrs-visualizer

    png_byte header[8];

    FILE *fp = fopen(file_name, "rb");
    if (fp == 0)
    {
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (width){ *width = temp_width; }
    if (height){ *height = temp_height; }

    //printf("%s: %lux%lu %d\n", file_name, temp_width, temp_height, color_type);

    if (bit_depth != 8)
    {
        fprintf(stderr, "%s: Unsupported bit depth %d.  Must be 8.\n", file_name, bit_depth);
        return 0;
    }

    GLint format;
    switch(color_type)
    {
    case PNG_COLOR_TYPE_RGB:
        format = GL_RGB;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        format = GL_RGBA;
        break;
    default:
        fprintf(stderr, "%s: Unknown libpng color type %d.\n", file_name, color_type);
        return 0;
    }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_byte ** row_pointers = (png_byte **)malloc(temp_height * sizeof(png_byte *));
    if (row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (unsigned int i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, temp_width, temp_height, 0, format, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;
}

bool valid_file(string file){
    int i;
    bool flag = true;
    string ext;

    if(file.empty() || file.size() < 4)
        return false;

    for(i = file.size()-1; i >= 0; i--){
        if(file[i] == '.') break;
        string f(1, file[i]);
        ext = f + ext;
    }

    return flag;
}

vector<string> list_datasets(bool list){
    vector<string> files;

    #ifdef __unix__
        DIR *dpdf;
        struct dirent *epdf;

        string path = "./Textures/Explosion";

        dpdf = opendir(path.c_str());
        if(dpdf != NULL){
            while((epdf = readdir(dpdf))){
                string file = string(epdf->d_name);
                if(valid_file(file) && !file.empty()){

                    if(list) cout << "[" << files.size() << "] " << file << endl;
                    files.push_back(file);
                }
            }
        }

        closedir(dpdf);
    #elif _WIN32
        HANDLE hFind;
        WIN32_FIND_DATA data;
        string path = ".\\Textures\Explosion\\*.*";

        hFind = FindFirstFile(path.c_str(), &data);
        if (hFind != INVALID_HANDLE_VALUE) {
          do {
            string file_name(data.cFileName);
            if(valid_file(file_name) && !file_name.empty()){
                if(list) cout << "[" << files.size() << "] " << file_name << endl;
                files.push_back(file_name);
            }
          } while (FindNextFile(hFind, &data));
          FindClose(hFind);
        }
    #else
        cout << "This system is not supported for this function..." << endl;
    #endif

    return files;
}
