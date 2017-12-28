#include "tracer.h"
#include "iostream"
#include "io.h"

void SetTracer(Tracer &tracer, INIReader reader){
    tracer.camera.position.x = reader.GetReal("camera", "posx", 0);
    tracer.camera.position.y = reader.GetReal("camera", "posy", 0);
    tracer.camera.position.z = reader.GetReal("camera", "posz", 0);

    tracer.h = reader.GetInteger("resolution", "h", 0);
    tracer.w = reader.GetInteger("resolution", "w", 0);

    std::cout << tracer.h << " " << tracer.w << std::endl;

    tracer.camera.viewAngle.y = reader.GetReal("viewAngle", "y", 0);
    tracer.camera.rotateAngle.x = reader.GetReal("rotateAngle", "x", 0);
    tracer.camera.rotateAngle.y = reader.GetReal("rotateAngle", "y", 0);
    tracer.GetViewAnglex();


    float right = tan((tracer.camera.viewAngle.x / 2.0) * PI / 180.0);
    tracer.camera.viewDir = glm::vec3(1.0f, 0.0f, 0.0f);
    tracer.camera.up = glm::vec3(0.0f, 0.0f, right * tracer.h / tracer.w);
    tracer.camera.right = glm::vec3(0.0f, right, 0.0f);

    tracer.constFong = reader.GetInteger("fong", "const", 1);
}

void SetAntenna(Antenna &antenna, INIReader reader){
    antenna.position.x = reader.GetReal("antenna", "posx", 0);
    antenna.position.y = reader.GetReal("antenna", "posy", 0);
    antenna.position.z = reader.GetReal("antenna", "posz", 0);

    antenna.power = reader.GetReal("antenna", "power", 1);
}



int main(int argc, char ** argv){
	if (argc != 3){
		std::cout << "Not enough arguments\n";
		return 1;
	}
	std::string settingsFile = argv[1];
	std::string objectFile = argv[2];
	INIReader reader(settingsFile.c_str());
    std::string outputImage = reader.Get("resultImage", "filename", "../out.bmp");
	if (reader.ParseError() > 0) {
        std::cout << "Can't load settingsFile\n";
        return 1;
    }
    Tracer tracer;
    SetTracer(tracer, reader);
    tracer.RotateCamera(tracer.camera.rotateAngle.x, tracer.camera.rotateAngle.y);
    tracer.LoadModel(objectFile);
    //
    VoxelGrid *tmpgrid_p = new VoxelGrid();
    VoxelGrid &tmpgrid = *tmpgrid_p;

    VoxelGrid *tmpgridSmooth_p = new VoxelGrid();
    VoxelGrid &tmpgridSmooth = *tmpgridSmooth_p;


    int size = tracer.objects.size();
    for (int i = 0; i < size; ++i){
        tmpgrid.objects.push_back(tracer.objects[i]);
        tmpgridSmooth.objects.push_back(tracer.objects[i]);
    }

    SetAntenna(tmpgrid.antenna, reader);
    SetAntenna(tmpgridSmooth.antenna, reader);

    tmpgrid.SetGridSize();
    tmpgridSmooth.SetGridSize();

    //SetAntenna(tracer.antenna, reader);
    //std::cout << "raytrace begin" << std::endl;
    tmpgrid.RayTrace();
    //std::cout << "raytrace end" << std::endl;
    //tracer.grid = tmpgrid;
    tmpgrid.BoxFilter(tmpgridSmooth);

    
    //std::cout << "11" << std::endl;
    Image resImage(tracer.h, tracer.w);

    //std::cout << "GetImage begin" << std::endl;
    resImage = tracer.GetImage(tmpgridSmooth);
    //std::cout << "GetImage begin" << std::endl;

    delete tmpgrid_p;
    save_image(resImage, "../out3.bmp");
	return 0;
}