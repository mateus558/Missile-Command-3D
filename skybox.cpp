#include <iostream>
#include <cstdio>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "skybox.h"

using namespace std;

SkyBox::SkyBox(const char* skybox_model){
	model.readFromFile(skybox_model);
	model.unitize();
	model.computeNormals();
}

void SkyBox::load_skybox(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right){
	int width, height;
	
	sides[0] = png_texture_load(front, &width, &height);
	sides[1] = png_texture_load(back, &width, &height);
	sides[2] = png_texture_load(top, &width, &height);
	sides[3] = png_texture_load(bottom, &width, &height);
	sides[4] = png_texture_load(left, &width, &height);
	sides[5] = png_texture_load(right, &width, &height);	
}

void SkyBox::load_skybox(const char* texture){
	int width, height;
	
	sides[0] = png_texture_load(texture, &width, &height);
	
	nTextures = 1;
}

void SkyBox::center_of(Object *obj){
	Point objCentroid = obj->getModel().getCentroid();
	Point boxCentroid = model.getCentroid();
	Point diff = boxCentroid - objCentroid;

	model.translate(diff);
}

void SkyBox::center_of(Point p){
	Point boxCentroid = model.getCentroid();
	Point diff = boxCentroid - p;

	model.translate(diff);
}

void SkyBox::draw_skybox(Point center, Point eye, Point pos, Point scale, Point rot){
	vector<vector<int> > faces = model.getFaces();
	vector<Point> points = model.getPoints();
	vector<Point> normals = model.getNormals();
	vector<UV> uv_coordinates = model.getUVCoords();
	int nf = 6, facePoints = 4, i, j, k = 0;
	
	glPushMatrix();
	glLoadIdentity();
		
		gluLookAt(0.56,0.4,-0.41,center.x, center.y,center.z,0,-1,0);
        
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDepthMask(GL_FALSE);
		
		glColor4f(1,1,1,1);
		
		glTranslatef(pos.x, pos.y, pos.z);
		
		glScalef(scale.x, scale.y, scale.z);
		glRotatef(rot.x, 1, 0, 0);
		glRotatef(rot.y, 0, 1, 0);
		glRotatef(rot.z, 0, 0, 1);
		glFrontFace(GL_CW);	
		if(nTextures == 1) glBindTexture(GL_TEXTURE_2D, sides[0]);
		for(i = 0; i < nf; i++){
			if(nTextures > 1) glBindTexture(GL_TEXTURE_2D, sides[i]);
			glBegin(GL_QUADS);
				for(j = 0; j < facePoints; j++){
					glTexCoord2f(uv_coordinates[faces[i][j]].u, uv_coordinates[faces[i][j]].v);
					glVertex3f(points[faces[i][j]].x, points[faces[i][j]].y, points[faces[i][j]].z);
				}
			glEnd();
		}
		
	glPopAttrib();
	glPopMatrix();
	
	
	glDepthMask(1); 
}

GLuint SkyBox::png_texture_load(const char * file_name, int * width, int * height)
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
   
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;
}
