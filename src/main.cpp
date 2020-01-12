#include "pch.h"

#include "core.h"

#include "actions.h"
#include "param.h"

#include "ipc/client.h"

#include "jsonfilewriter.h"
#include "jsonfilereader.h"

// for explorerDialog
#include <dirent.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/stat.h>



#include <regex>

#include "wave_file_reader.h"
#include <sndfile.h>

#include "fft.h"


/*******************************************************************/





// forward declarations ...
void display_fft_values();
void register_action(std::shared_ptr<Action> action);

std::shared_ptr<Module> add_module(MODULE_TYPE type, unsigned int layer_num = 0);
void remove_module(unsigned int id);

struct ReleaseData{	
	virtual ~ReleaseData(){};
	virtual void funcA() = 0;
	
	std::shared_ptr<BaseParam> parent_param = nullptr;
};
struct ReleaseDataFloat : public ReleaseData{
	inline void funcA() override{}
	float old_val;
	float new_val;
	std::function<void()> callback;
};
struct ReleaseDataInt : public ReleaseData{
	inline void funcA() override{}
	int old_val;
	int new_val;
	std::function<void()> callback;
};
struct ReleaseDataColor3 : public ReleaseData{
	inline void funcA() override{}
	glm::vec3 old_val;
	glm::vec3 new_val;
	std::function<void()> callback;
};
struct ReleaseDataSignalRange : public ReleaseData{
	inline void funcA() override{}
	SignalRange old_val;
	SignalRange new_val;
	std::function<void()> callback;
};


enum FILE_EXPLORER_MODE
{
	FILE_EXPLORER_MODE_LOAD_FILE,
	FILE_EXPLORER_MODE_SAVE_FILE,
	FILE_EXPLORER_MODE_DIRECTORY
};

GLFWwindow * ui_window;
GLFWwindow * live_window;

GLuint viewport_FBO = 0;
GLuint viewport_FBO_VBO = 0;
GLuint viewport_FBO_texture = 0;

unsigned int w_width = 1280;
unsigned int w_height = 720;

unsigned int live_w_width = 640;
unsigned int live_w_height = 360;


unsigned int bufid; 
Shader fbo_shader;

ImFont* small_font;
bool signal_range_dialog_opened = true;

std::shared_ptr<Timer> timer = std::make_shared<Timer>();

int counter = 0;

ParamLayout player_layout("Player Functions");
std::vector<std::shared_ptr<BaseParam> > params; 
std::shared_ptr<BaseParam> active_param = nullptr;
bool is_param_clicked = false;
std::shared_ptr<ReleaseData> current_param_data;

std::vector<std::shared_ptr<Module>> modules;

std::vector< std::shared_ptr<Action> > actions;
std::vector< std::shared_ptr<Action> > actions_redos;


//~ std::string current_explorer_path = "/home/pi/";
static bool b_explorer_opened = false;
std::function<void()> current_explorer_callback = [](){};
//~ std::string current_explorer_file_path = "";
std::string current_scene_file_name = "default.vjs";

std::string current_scene_file_path = "";


std::string explorer_V2_dir_path = "/home/pi/Downloads/";
std::string explorer_V2_file_name = "";
std::string current_explorer_file_path_V2 = "";
int current_module_id = -1;



float sound_buffer[512 * 2];
fftw_complex * fft_out;

const unsigned int NUM_BANDS = 32;
float fft_maximums[NUM_BANDS];
float fft_values[NUM_BANDS];
float fft_threshold = 0.01f;
float fft_exponent = 1.5f;
Renderer renderer( fft_maximums );

WaveFileReader wave_reader(sound_buffer);
SOUND_PLAYER_CMD sound_player_cmd = SOUND_PLAYER_CMD_PLAY;
SOUND_PLAYER_MODE sound_player_mode = SOUND_PLAYER_MODE_FILE;
char * WAV_PATH;

float sine_wave_frequency = 440.0f;


// noise
FastNoise noise;
GLuint noise_texture;
const size_t noise_texture_size = 256;
float noise_frequency = 1.0;
unsigned char noise_data[noise_texture_size*noise_texture_size * 4];
bool noise_trigger = false;
bool noise_trigger_update = false;


std::vector<std::shared_ptr<BaseParam> > pinned_params; //// ????
// sound player params
std::shared_ptr<ParamFilePath> sound_player_wave_file_path_param = std::make_shared<ParamFilePath>();


void generate_noise(float freq)
{

	
	
	noise.SetFrequency( noise_frequency);
	for (size_t y = 0; y < noise_texture_size; y++)
	{
		//~ std::cout << "generating noise" << std::endl;
		for (size_t x = 0; x < noise_texture_size; x++)
		{
			unsigned char noise_val = (unsigned char)(((noise.GetNoise((float)x /noise_texture_size, (float)y /noise_texture_size) + 1.0f) / 2.0f) * 255);
			noise_data[(x*4) + ( y * noise_texture_size * 4) + 0] = noise_val;
			noise_data[(x*4) + ( y * noise_texture_size * 4) + 1] = noise_val;
			noise_data[(x*4) + ( y * noise_texture_size * 4) + 2] = noise_val;
			noise_data[(x*4) + ( y * noise_texture_size * 4) + 3] = 255;
		}
	}
	
	
	

}

static void update_noise_texture()
{
	if(noise_texture)
		GLCall(glDeleteTextures(1, &noise_texture));
	
	GLCall(glGenTextures(1, &noise_texture)); // Generate noise texture
	
	GLCall(glBindTexture(GL_TEXTURE_2D, noise_texture)); // Bind the texture fbo_texture

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, noise_texture_size, noise_texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, &noise_data[0])); // Create a standard texture with the width and height of our window

	// Setup the basic texture parameters
	GLCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

	// Unbind the texture
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));		
}

void noise_threaded( bool * trigger = nullptr)
{
	while(true)
	{
		if(noise_trigger)
		{
			generate_noise(noise_frequency);
			
			noise_trigger = false;
			*trigger = true;
		}
		
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	
}


void noise_dialog()
{
	if(ImGui::Begin("Fast Noise"), true)
	{
		if(ImGui::DragFloat("Frequency", &noise_frequency, 0.05f ))
		{
			noise_trigger = true;
		}
		
		const char * noise_types[3] = {"PerlinFractal", "SimplexFractal", "Cellular"};
		
		static int choice = 0;
		if(ImGui::BeginCombo("type",noise_types[choice]))
		{
			
			if(ImGui::Selectable(noise_types[0], choice == 0))
			{
				noise.SetNoiseType(FastNoise::PerlinFractal);
				
				noise_trigger = true;
				choice = 0;
			}
			if(ImGui::Selectable(noise_types[1], choice == 1))
			{
				noise.SetNoiseType(FastNoise::SimplexFractal);
				noise_trigger = true;
				choice = 1;
			}			
			if(ImGui::Selectable(noise_types[2], choice == 2))
			{
				noise.SetNoiseType(FastNoise::Cellular);
				noise_trigger = true;
				choice = 2;
			}				
			ImGui::EndCombo();
		}
		if(ImGui::Button("Generate"))
		{
			//~ noise_trigger = true;
			update_noise_texture();
		}
		
		int avail_width = ImGui::GetContentRegionAvail().x;
		ImGui::Image((void*)(uintptr_t)noise_texture, ImVec2(avail_width,(int)((float)avail_width)),ImVec2(0,1), ImVec2(1,0));	
		
		ImGui::End();
	}
}

std::vector<std::string> split(const std::string& str, std::string delimiter = " ")
{
	
	std::vector<std::string> tokens;
	std::string tmp = str;
	//~ std::string::size_type start = 0;
	while(true)
	{
		std::string::size_type found = tmp.find_first_of(delimiter);
		if( found == std::string::npos )
		{
			//std::cout << " reached end ------------------------------- " << std::endl;	
			tokens.push_back(tmp);
			
			break;
		}else{
			std::string cut = tmp.substr(0, found);
			tokens.push_back(cut);
			tmp = tmp.substr(found+1, tmp.size() - (found-1));
			//std::cout << tmp << std::endl;
			//~ start = found;
		}
	}
	
	//std::cout << " ------------------------------- " << std::endl;	
	
	return tokens;
}

int explorerDialog_V2(const char * path = "", const char * file_name = "")
{
	
	using namespace std;
	DIR* dir;
	struct dirent *sd;
	
	
	
	struct stat st_buf;
	int status;
	
	//~ if(path != "" )
		//~ current_explorer_path = std::string(path);

	if((dir = opendir(explorer_V2_dir_path.c_str())) == NULL){ /*Opens directory*/
		//~ printf("explorer error \n \t%s \n", explorer_V2_dir_path.c_str());
		return errno;
	}

	std::vector<std::string> dir_names;
	std::vector<std::string> file_names;
	
	// collect files and dirs names
	while ((sd= readdir(dir)) != NULL){ /*starts directory stream*/
		if(strcmp(sd -> d_name,".") == 0 || strcmp(sd -> d_name,"..") == 0)
		{
			continue;
		}else{			
			std::string check_path = explorer_V2_dir_path;
			check_path += sd->d_name;	
				
			status = stat (check_path.c_str(), &st_buf);
				
			if (!S_ISREG(st_buf.st_mode)) 
			{		
				if(sd->d_name[0] != '.')	
					dir_names.push_back(sd->d_name);
			}else{
				if(sd->d_name[0] != '.')	
					file_names.push_back(sd->d_name);
			}
		}
	}  
	closedir(dir); /* important !!!! */
	
	
	
	// sort names vectors
	sort(dir_names.begin(), dir_names.end(), 
	[](std::string& str1, std::string& str2)
	{ 
		std::string lower1 = str1;
		std::string lower2 = str2;
		std::transform(lower1.begin(), lower1.end(), lower1.begin(), ::tolower);
		std::transform(lower2.begin(), lower2.end(), lower2.begin(), ::tolower);
		return lower1 < lower2;
	});
	sort(file_names.begin(), file_names.end(), 
	[](std::string& str1, std::string& str2)
	{ 
		std::string lower1 = str1;
		std::string lower2 = str2;
		std::transform(lower1.begin(), lower1.end(), lower1.begin(), ::tolower);
		std::transform(lower2.begin(), lower2.end(), lower2.begin(), ::tolower);
		return lower1 < lower2;
	});	
	
	
	
	if(ImGui::Begin("File explorer 2", &b_explorer_opened))
	{
		
		static char file_n[256] = "hello\0";
		//~ strcpy(file_n,current_scene_file_name.c_str());
		if(ImGui::InputText(":file name", file_n, IM_ARRAYSIZE(file_n))){
			//~ current_scene_file_name = file_n;
		}
		ImGui::SameLine();
		if(ImGui::Button("Save")){
			b_explorer_opened = false;
			
			std::string f_name_str(file_n);
			
			current_explorer_file_path_V2 = explorer_V2_dir_path + f_name_str;
			current_explorer_callback();			
			//~ printf("current path : %s\n", current_explorer_file_path.c_str());
		}
		
		// make directories a different color
		ImVec4 color = ImColor(1.0f, 1.0f, 0.2f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		
		if(ImGui::Selectable("..", false))
		{
			
			if( explorer_V2_dir_path != "/")
			{
				std::string new_path;
				new_path = "";
				
				std::vector<std::string> splitted = split(explorer_V2_dir_path, "/");
				
				for (size_t i = 0; i < splitted.size()-2; i++)
				{
					new_path += splitted[i];
					new_path += "/";
				}
				
				//~ std::cout << "\n";
					
				explorer_V2_dir_path = new_path;		
			}
		}
		for (size_t i = 0; i < dir_names.size(); i++)
		{
			if(ImGui::Selectable(dir_names[i].c_str(), false)){
				
				explorer_V2_dir_path += dir_names[i];	
				explorer_V2_dir_path += "/";		
			}	
		}
		// revert back to default color
		ImGui::PopStyleColor();
		
		for (size_t i = 0; i < file_names.size(); i++)
		{
			if(ImGui::Selectable(file_names[i].c_str(), false)){

				explorer_V2_file_name = file_names[i];
				//~ printf("Selected File is : %s / %s\n", explorer_V2_dir_path.c_str(), explorer_V2_file_name.c_str());
				strcpy(&file_n[0], explorer_V2_file_name.c_str());
			}	
		}	
		
		
		
		
		ImGui::End();
	}
	
	
	dir_names.clear();
	file_names.clear();
	
	return 0;
}

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
{
	printf("STUB !!! \n\n");
	return 42;
}

static std::string increment_name(std::string _name)
{
	std::string s = _name;
	std::smatch m;
	std::regex e("\\d+?$");
	bool found = std::regex_search(_name, m, e);
	if(found)
	{
		int num = std::stoi(m.str());
		for(size_t i=0; i < std::strlen(m.str().c_str()); i++){
			s.pop_back();
		}
		printf("found digit ! %d\n", num);
		s += std::to_string(num+1);
	}else{
		s += "2";
	}
		
	return s;
}

std::string uniqueName(std::string _str)
{
	

	for(auto module : renderer.m_modules)
	{
		if(_str == module->getName())
		{
			//~ printf("same name \n");
			_str = increment_name(_str);
			_str = uniqueName(_str);
			break;
		}
	}
	
	return _str;
}


void UI_widget(std::shared_ptr<BaseParam> param_ptr, std::shared_ptr<BaseParam> parent_param = nullptr, std::function<void()> callback = [](){})
{
	
	float LABEL_WIDTH = ImGui::GetContentRegionAvailWidth() * 0.25f;
	
	Param<float> * p_float  = nullptr;
	Param<int> *   p_int    = nullptr;
	Param<bool> *  p_bool   = nullptr;
	Param<std::string> *  p_text   = nullptr;
	ParamInfo *  p_info   = nullptr;
	Param<SignalRange> *  p_signal_range   = nullptr;
	ParamMenu *    p_menu   = nullptr;
	ParamButton *  p_button = nullptr;
	ParamColor3 *  p_color3 = nullptr;
	ParamSeparator *  p_separator = nullptr;
	ParamFilePath *  p_file_path = nullptr;

	ImGui::PushFont(small_font);
	ImGui::PushID(param_ptr->getName());
	char _name[255];
	      if((p_float  = dynamic_cast<Param<float> *>(param_ptr.get())) ){
		
		float new_val = p_float->getValue();
		float old_val = new_val;

		
		sprintf((char *)_name, "##%s", p_float->getName());

		ImGui::Columns(3);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text(p_float->getName() );
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if( ImGui::DragFloat((const char *) _name, &new_val, 0.05f))
		{
			//~ p_float->setValue(_val);
			int state = glfwGetMouseButton(ui_window, GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS)
			{
				if( !is_param_clicked){
					
					active_param = param_ptr;
					is_param_clicked = true;
					
					std::shared_ptr<ReleaseDataFloat> data = std::make_shared<ReleaseDataFloat>();
					current_param_data = data;
					data->old_val = old_val; 
					data->callback = [](){
						//~ printf("Float Param Release\n");
					};					
				}
				
				
				ReleaseDataFloat * data_float = nullptr;
				if( (data_float = dynamic_cast<ReleaseDataFloat *>( current_param_data.get()))){
					
					data_float->new_val = new_val; 
					
				}
				p_float->setValue(new_val);
			}else{
				
					std::shared_ptr<ActionParamChange<float> > action = std::make_shared<ActionParamChange<float> >(p_float, old_val, new_val, [](){
						//~ printf("Float action callback !!!!!!\n");
					});
					//~ actions.insert(actions.begin(), action );
					register_action(action);
					//~ p_float->setValue(new_val);
					
			}
		
			
		}
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		
		bool checked = param_ptr->getUseSignalRange();
		if(ImGui::Checkbox(" : Use Signal", &checked)){
			
			bool old_val = p_float->getUseSignalRange();
			bool new_val = checked;
			std::shared_ptr<ActionParamUseSignalChange > action = std::make_shared<ActionParamUseSignalChange >(p_float, old_val, new_val, [](){
				//~ printf("Use Signal Action Log\n");
			});
			//~ actions.insert(actions.begin(), action );	
			register_action(action);		
			
			param_ptr->setUseSignalRange( checked);
		}
		ImGui::Columns(1);
		
		

				
		
	}else if((p_int    = dynamic_cast<Param<int> *>  (param_ptr.get())) ){
		int _val = p_int->getValue();
		int old = _val;

		sprintf((char *)_name, "##%s", p_int->getName());		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text(p_int->getName() );
		ImGui::NextColumn();	
		ImGui::PushItemWidth(-1);	
		if( ImGui::SliderInt((const char *) _name, &_val, 0, 20)){
			int state = glfwGetMouseButton(ui_window, GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS)
			{
				if( !is_param_clicked){
					
					active_param = param_ptr;
					is_param_clicked = true;
					
					std::shared_ptr<ReleaseDataInt> data = std::make_shared<ReleaseDataInt>();
					current_param_data = data;
					data->old_val = old; 
					data->callback = [](){
						
					};					
				}
				
				
				
				ReleaseDataInt * data_int = nullptr;
				if(( data_int = dynamic_cast<ReleaseDataInt *>( current_param_data.get()))){
					
					data_int->new_val = _val; 
					
				}
				p_int->setValue(_val);

			}
		}
		
		ImGui::Columns(1);
	}else if((p_bool   = dynamic_cast<Param<bool> *> (param_ptr.get())) ){
		bool _val = p_bool->getValue();
		bool old = _val;

		sprintf(_name, "##%s", p_bool->getName());		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text(p_bool->getName() );
		ImGui::NextColumn();	
		ImGui::PushItemWidth(-1);	
		if( ImGui::Checkbox( _name, &_val)){
			std::shared_ptr<ActionParamChange<bool> > action = std::make_shared<ActionParamChange<bool> >(p_bool, old, _val, [](){
					
			});
			//~ actions.insert(actions.begin(), action );
			register_action(action);
		}
		ImGui::Columns(1);
		
	}else if((p_text   = dynamic_cast<Param<std::string> *> (param_ptr.get())) ){
		//~ ImGui::Text(p_text->getValue());
		std::string _val = p_text->getValue();
		std::string old = std::string(_val);

		sprintf(_name, "##%s", p_text->getName());		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text(p_text->getName() );
		ImGui::NextColumn();	
		ImGui::PushItemWidth(-1);	
		
		

		static char temp_char[256];
		strcpy(temp_char,_val.c_str());		
		if( ImGui::InputText( _name, temp_char, 256, 
			ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub))
		{
			std::shared_ptr<ActionParamChange<std::string> > action = std::make_shared<ActionParamChange<std::string> >(p_text, old, std::string(temp_char), [](){
					
					
			});
			//~ actions.insert(actions.begin(), action );
			register_action(action);
			p_text->setValue(temp_char);
		}
		ImGui::Columns(1);
		
	}else if((p_info   = dynamic_cast<ParamInfo *> (param_ptr.get()))){
		sprintf(_name, "##%s", p_info->getName());		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text(p_info->getName() );
		ImGui::NextColumn();	
		ImGui::PushItemWidth(-1);	
		ImGui::Text((char *)p_info->getValue().data());
		
		ImGui::PopItemWidth();	
		
	}else if((p_signal_range = dynamic_cast<Param<SignalRange > *>(param_ptr.get()))){
	
		SignalRange new_val = parent_param->getSignalRange();
		SignalRange old_val = new_val;
		int new_val_min = parent_param->getSignalRange().min;
		int new_val_max = parent_param->getSignalRange().max;
		
		float new_multiplier = parent_param->getSignalRange().multiplier;
		//~ SignalRangeMode new_mode = parent_param->getSignalRange().mode;
		
		
		ImGui::Text(parent_param->getName() );
		ImGui::Separator();
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text("Min Band :" );
		
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		
		ImGui::PushID(p_signal_range);
		
		sprintf((char *)_name, "##%s --> min", parent_param->getName());

		if( ImGui::SliderInt((const char *) _name, &new_val_min, 0, 31))
		{
			
			if( new_val_min >= new_val_max && new_val_min < (int)NUM_BANDS)
			{
				new_val_max = new_val_min + 1;
			}
			if(new_val_min >= (int)NUM_BANDS - 1){
				new_val_min = (int)NUM_BANDS - 2;
			}
			
			int state = glfwGetMouseButton(ui_window, GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS)
			{
				if( !is_param_clicked){
					
					active_param = param_ptr;
					is_param_clicked = true;
					
					std::shared_ptr<ReleaseDataSignalRange> data = std::make_shared<ReleaseDataSignalRange>();
					current_param_data = data;
					data->old_val = old_val; 
					data->callback = [](){
						
					};					
				}
				
				
				ReleaseDataSignalRange * data_signal_range = nullptr;
				if(( data_signal_range = dynamic_cast<ReleaseDataSignalRange *>( current_param_data.get()))){
					SignalRange result = parent_param->getSignalRange();
					result.min = new_val_min;
					//~ result.max = new_val_max;
					//~ p_signal_range->setValue(result);					
					data_signal_range->new_val = result; 
					data_signal_range->parent_param = parent_param;
						
					parent_param->setSignalRange(result);
				}
				
				
				

			}else{
				
				SignalRange result;
				result.min = new_val_min;
				result.max = new_val_max;

				

					std::shared_ptr<ActionParamSignalRangeChange > action = std::make_shared<ActionParamSignalRangeChange >(parent_param.get(), old_val, result, [](){
			
					});
					//~ actions.insert(actions.begin(), action );
					register_action(action);

			}
		
			
		}
		//~ ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::Columns(1);
	
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text("Max Band :" );
		
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		
		//~ ImGui::PushID(p_signal_range);
		
		sprintf((char *)_name, "##%s --> max", parent_param->getName());

		if( ImGui::SliderInt((const char *) _name, &new_val_max, 0, 31))
		{
			if( new_val_max <= new_val_min && new_val_max > 0)
			{
				new_val_min = new_val_max - 1;
			}	
			
			if(new_val_max < 1){
				new_val_max = 1;
			}
			int state = glfwGetMouseButton(ui_window, GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS)
			{
				if( !is_param_clicked){
					
					active_param = param_ptr;
					is_param_clicked = true;
					
					std::shared_ptr<ReleaseDataSignalRange> data = std::make_shared<ReleaseDataSignalRange>();
					current_param_data = data;
					data->old_val = old_val; 
					data->callback = [](){
						//~ printf("Signal Range Maximum Param Release\n");
					};					
				}
				
				
				ReleaseDataSignalRange * data_signal_range = nullptr;
				if( (data_signal_range = dynamic_cast<ReleaseDataSignalRange *>( current_param_data.get()))){
					SignalRange result = parent_param->getSignalRange();
					//~ result.min = new_val_min;
					result.max = new_val_max;
					//~ p_signal_range->setValue(result);					
					data_signal_range->new_val = result; 
					data_signal_range->parent_param = parent_param;
						
					parent_param->setSignalRange(result);
				}
				
				
				

			}else{
				
				SignalRange result = parent_param->getSignalRange();
				result.min = new_val_min;
				result.max = new_val_max;
			

				std::shared_ptr<ActionParamSignalRangeChange > action = std::make_shared<ActionParamSignalRangeChange >(parent_param.get(), old_val, result, [](){
					
				});
				//~ actions.insert(actions.begin(), action );
				register_action(action);

			}
		
			
		}
		//~ ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::Columns(1);	
		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text("Mode" );
		
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);		
		
		int choice = parent_param->getSignalRange().mode;
		const char * modes[3] = {"Multiply", "Add", "Around"};
		sprintf((char *)_name, "##%s --> signal_modes", parent_param->getName());
		if(ImGui::BeginCombo((const char *)_name, modes[choice]))
		{
			if(ImGui::Selectable("Multiply"))
			{
				choice = 0;
				SignalRange result = parent_param->getSignalRange();
				result.mode = (SignalRangeMode)choice;			

				std::shared_ptr<ActionParamSignalRangeChange > action = std::make_shared<ActionParamSignalRangeChange >(parent_param.get(), old_val, result, [](){
					
				});
				//~ actions.insert(actions.begin(), action );	
				register_action(action);
			}
			
			if(ImGui::Selectable("Add"))
			{
				choice = 1;
				SignalRange result = parent_param->getSignalRange();
				result.mode = (SignalRangeMode)choice;			

				std::shared_ptr<ActionParamSignalRangeChange > action = std::make_shared<ActionParamSignalRangeChange >(parent_param.get(), old_val, result, [](){
					
				});
				//~ actions.insert(actions.begin(), action );		
				register_action(action);			
			}
			
			if(ImGui::Selectable("Around"))
			{
				choice = 2;
				SignalRange result = parent_param->getSignalRange();
				result.mode = (SignalRangeMode)choice;			

				std::shared_ptr<ActionParamSignalRangeChange > action = std::make_shared<ActionParamSignalRangeChange >(parent_param.get(), old_val, result, [](){
					
				});
				//~ actions.insert(actions.begin(), action );		
				register_action(action);			
			}
			ImGui::EndCombo();
		}
		ImGui::Columns(1);
		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text("Multiplier" );
		
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		
		sprintf((char *)_name, "##%s --> multiplier", parent_param->getName());
		
		if( ImGui::SliderFloat((const char *)_name, &new_multiplier, 0.0f , 4.0f)){
			int state = glfwGetMouseButton(ui_window, GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS)
			{
				if( !is_param_clicked){
					
					active_param = param_ptr;
					is_param_clicked = true;
					
					std::shared_ptr<ReleaseDataSignalRange> data = std::make_shared<ReleaseDataSignalRange>();
					current_param_data = data;
					data->old_val = old_val; 
					data->callback = [](){
						//~ printf("Signal Range Maximum Param Release\n");
					};					
				}
				
				
				ReleaseDataSignalRange * data_signal_range = nullptr;
				if( (data_signal_range = dynamic_cast<ReleaseDataSignalRange *>( current_param_data.get()))){
					SignalRange result = parent_param->getSignalRange();
					result.multiplier = new_multiplier;
					//~ p_signal_range->setValue(result);					
					data_signal_range->new_val = result; 
					data_signal_range->parent_param = parent_param;
						
					parent_param->setSignalRange(result);
				}
				
				
				

			}else{
				SignalRange result = parent_param->getSignalRange();
				result.multiplier = new_multiplier;
			

				std::shared_ptr<ActionParamSignalRangeChange > action = std::make_shared<ActionParamSignalRangeChange >(parent_param.get(), old_val, result, [](){
					//~ printf("Signal Range Maximum action Input callback !!!!!!\n");
				});
				//~ actions.insert(actions.begin(), action );	
				register_action(action);			
				
			}
		}
		
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();
		
	}else if((p_menu   = dynamic_cast<ParamMenu *>   (param_ptr.get()) )){
		
		
		sprintf(_name, "##%s", p_menu->getName());	
				
		int choice = p_menu->getValue();
		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text(p_menu->getName() );
		ImGui::NextColumn();	
		ImGui::PushItemWidth(-1);
		

							
		if(ImGui::BeginCombo(_name, p_menu->entries[choice].first,0))
		{
			int inc = 0;
			for(auto entry : p_menu->entries)
			{
				if(ImGui::Selectable(entry.first, choice == inc))
				{
					
					int _val = p_menu->getValue();
					int old = _val;					
					
					std::shared_ptr<ActionParamMenuChange> action = std::make_shared<ActionParamMenuChange >(p_menu, old, inc, [old, inc](){
							//~ printf("Menu action callback old : %d, new %d\n", old, inc);
					});
					//~ actions.insert(actions.begin(), action );
					register_action(action);
					choice = inc;
				}
				inc++;
			}
			ImGui::EndCombo();
		}
		
		ImGui::Columns(1);
		
	}else if((p_button = dynamic_cast<ParamButton *> (param_ptr.get()) )){
		
		
		sprintf(_name, "##%s", p_button->getName());	
			
		//~ printf("##%s", p_button->getName());	

		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text(" ");
		ImGui::NextColumn();	
		ImGui::PushItemWidth(-1);	
		
		if(ImGui::Button(p_button->getName())){
			p_button->getCallback()();
		}	
		
		ImGui::Columns(1);
	}else if((p_color3 = dynamic_cast<ParamColor3 *> (param_ptr.get()) )){
		
		glm::vec3 _val = glm::vec3(p_color3->getValue());
		glm::vec3 old = _val;		
		
		sprintf(_name, "##%s", p_color3->getName());	
			
		//~ printf("##%s", p_color3->getName());	

		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LABEL_WIDTH);
		ImGui::Text(p_color3->getName());
		ImGui::NextColumn();	
		ImGui::PushItemWidth(-1);	
		
		
		glm::vec3 temp_color  = glm::vec3(p_color3->color.x, p_color3->color.y, p_color3->color.z);
		if(ImGui::ColorEdit3("##u_color", (float*)glm::value_ptr(temp_color)))
		{
			int state = glfwGetMouseButton(ui_window, GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS)
			{
				if( !is_param_clicked){
					
					active_param = param_ptr;
					is_param_clicked = true;
					
					std::shared_ptr<ReleaseDataColor3> data = std::make_shared<ReleaseDataColor3>();
					current_param_data = data;
					data->old_val = old; 
					data->callback = [](){
						//~ printf("Color Param Release\n");
						//~ p_color3->color = 
					};					
					
					//~ printf("Clicked, old value = %.3f %.3f %.3f \n", old.x, old.y, old.z);
				}
				
				
				ReleaseDataColor3 * data_color3 = nullptr;
				if(( data_color3 = dynamic_cast<ReleaseDataColor3 *>( current_param_data.get()))){
					
					data_color3->new_val = temp_color; 
					
				}
				//~ p_color3->color = _val;
				
			}else{
				
					std::shared_ptr<ActionParamColor3Change> action = std::make_shared<ActionParamColor3Change>(p_color3, old, temp_color, [](){
						//~ printf("Color3 action callback !!!!!!\n");
					});
					//~ actions.insert(actions.begin(), action );
					register_action(action);
					p_color3->color = temp_color;
			}
			
					
		}

		
		ImGui::Columns(1);
	}else if((p_separator = dynamic_cast<ParamSeparator *>(param_ptr.get()))){
		ImGui::Separator();
	}else if((p_file_path = dynamic_cast<ParamFilePath *>(param_ptr.get()))){
		std::string _val = p_file_path->getValue();
		std::string old = _val;

		sprintf(_name, "##%s", p_file_path->getName());		
		ImGui::Columns(3);
		ImGui::PushItemWidth(-1);	
		ImGui::Text(p_file_path->getName() );
		ImGui::NextColumn();	
		ImGui::PushItemWidth(-1);	
		
		

	
		ImGui::Text((char *)p_file_path->getValue().c_str());
		
		
		
		ImGui::NextColumn();	
		if(ImGui::Button("file") == true)
		{
				b_explorer_opened = true;
				_val = explorer_V2_dir_path + explorer_V2_file_name;
				current_explorer_callback = [p_file_path, old, _val](){
					//~ printf("ParamFilePath -->  current path : %s\n" ,explorer_V2_dir_path.c_str());
					
					p_file_path->setValue(explorer_V2_dir_path + explorer_V2_file_name);
					std::shared_ptr<ActionParamFilePathChange > action = std::make_shared<ActionParamFilePathChange >(p_file_path, old, p_file_path->getValue(), p_file_path->getCallback());	
					
					
					current_explorer_callback = [](){};
					//~ actions.insert(actions.begin(), action );
					register_action(action);
				};			
		}
		ImGui::Columns(1);		
		
		
	}

	ImGui::PopID();
	ImGui::PopFont();
	
	ImGui::Columns(1);
	ImGui::Spacing();
	
}

void register_action(std::shared_ptr<Action> action)
{
	actions.insert(actions.begin(), action);
	actions_redos.clear();
}

void action_undo()
{
	if(actions.size() > 0)
	{
		actions[0]->undo();
		actions_redos.insert(actions_redos.begin(), actions[0]);
		actions.erase(actions.begin());
	}	
}

void action_redo()
{
	if(actions_redos.size() > 0)
	{
		actions_redos[0]->redo();
		actions.insert(actions.begin(), actions_redos[0]);
		actions_redos.erase(actions_redos.begin());
	}		
}

void actions_dialog()
{
	
	if(ImGui::Begin("Actions"), true)
	{
		ImGui::Columns(2);
		ImGui::Text("Undos");
		ImGui::PushItemWidth(-1);
		if (ImGui::ListBoxHeader("##Undos", 10))
		{
			for(auto action : actions)
			{
				ImGui::Selectable(action->getName(), false);
			}
			
			ImGui::ListBoxFooter();
		}

		ImGui::NextColumn();
		ImGui::Text("Redos"); 
		ImGui::PushItemWidth(-1);
		if (ImGui::ListBoxHeader("##Redos", 10))
		{
			for(auto action : actions_redos)
			{
				ImGui::Selectable(action->getName(), false);
			}
			
			ImGui::ListBoxFooter();
		}        

		ImGui::Columns(1);
		
		if(ImGui::Button("Clear History"))
		{
			actions_redos.clear();
			actions.clear();
			
		}

		ImGui::End();
	}
}

void saveToFile()
{
	//~ printf("\nsave function\n\n");
	
		
	JsonFileWriter writer;
	writer.encodeModules(renderer.m_modules);
	
	current_scene_file_path = current_explorer_file_path_V2;
	
	std::ofstream output_file;
	output_file.open(current_explorer_file_path_V2);
	
	
	
	output_file << writer.json_data; ;
	output_file.close();
	
	b_explorer_opened = false;
	
	if( current_scene_file_path != "" )
	{
		glfwSetWindowTitle(ui_window, current_scene_file_path.c_str()); 
	}
}

void loadFromFile()
{
	JsonFileReader reader;
	VJ_FILE_DATA data = reader.load(current_explorer_file_path_V2, timer);
	renderer.m_modules = data.modules; 
	
	//~ for(auto module : renderer.m_modules)
	//~ {
		//~ module->setTimer(timer);
	//~ }
	//~ std::cout << current_explorer_file_path_V2 << std::endl;
}

void move_module( int from , int to)
{

	
	std::shared_ptr<Module > save = renderer.m_modules[from];
	renderer.m_modules.erase( renderer.m_modules.begin() + from);	
	renderer.m_modules.insert( renderer.m_modules.begin() + to, save);
	
	// follow layer selection if it was already selected
	//~ if(current_module_id == from)
		current_module_id = to;
}

std::shared_ptr<Module> add_module(MODULE_TYPE type, unsigned int layer_num)
{


	if( (MODULE_TYPE)type == MODULE_TYPE_ORBITER)
	{
		
		std::shared_ptr<Orbiter> mod = std::make_shared<Orbiter>(timer);
		mod->setName(uniqueName("Orbiter"));
		mod->p_color->color = glm::vec3(1.0, 1.0, 1.0);
		mod->init();
		renderer.m_modules.insert(renderer.m_modules.begin() + layer_num, mod);
		
		return mod;
	}
	else if( (MODULE_TYPE)type == MODULE_TYPE_CIRCLES)
	{
		std::shared_ptr<Circles> mod = std::make_shared<Circles>(timer);
		mod->setName(uniqueName("Circles"));
		mod->p_color->color = glm::vec3(1.0, 0.0, 1.0);
		mod->init();
		renderer.m_modules.insert(renderer.m_modules.begin() + layer_num, mod);
		
		return mod;
	}	
	else if( (MODULE_TYPE)type == MODULE_TYPE_IMAGE)
	{
		std::shared_ptr<Image> mod = std::make_shared<Image>(timer);
		mod->setName(uniqueName("Image"));
		mod->p_color->color = glm::vec3(1.0, 1.0, 1.0);
		mod->init();
		renderer.m_modules.insert(renderer.m_modules.begin() + layer_num, mod);
		
		return mod;
	}else if( (MODULE_TYPE)type == MODULE_TYPE_PARTICLES)
	{
		std::shared_ptr<Particles> mod = std::make_shared<Particles>(timer);
		mod->setName(uniqueName("Particles"));		
		mod->init();
		
		renderer.m_modules.insert(renderer.m_modules.begin() + layer_num, mod);
		return mod;
	}	
	
	current_module_id = layer_num;
	
	return nullptr;
}

void add_module_ptr(std::shared_ptr<Module> ptr, unsigned int layer_num = 0)
{				
	renderer.m_modules.insert(renderer.m_modules.begin() + layer_num, ptr);
}


void duplicate_module(std::shared_ptr<Module> ptr)
{
	
	
	Orbiter * p_orbiter = nullptr;
	
	if((p_orbiter = dynamic_cast<Orbiter *>(ptr.get())))
	{
		std::shared_ptr<Orbiter> copy = std::make_shared<Orbiter>(*(p_orbiter));
		copy->setTimer(timer);
		copy->setName(uniqueName(p_orbiter->getName()));
		copy->init();
		
		renderer.m_modules.insert(renderer.m_modules.begin() + 0, copy);
		
		
	}
	
	
	
	
	printf("what ?!\n");
}
void remove_module(unsigned int id)
{
	renderer.m_modules.erase(renderer.m_modules.begin() + id, renderer.m_modules.begin() + id + 1);
	if(renderer.m_modules.size() == 0)
		current_module_id = -1;
}

void remove_module_ptr(std::shared_ptr<Module> module_ptr)
{
	auto it = std::find(renderer.m_modules.begin(), renderer.m_modules.end(), module_ptr);
	
	if( it != renderer.m_modules.end())
	{
		
		int index = std::distance(renderer.m_modules.begin(), it);
		remove_module(index);
	}
}

void module_list_dialog()
{
	using namespace ImGui;
	if( ImGui::Begin("Module List"), true)
	{
		const char * choices[] = {"Orbiter", "Circles", "Image", "Particles"};
		static int choice = 1;
		SetNextItemWidth(100);
		if(BeginCombo("##label", choices[choice - 1])){
			
			//~ if(Selectable("choose")){
				
			//~ }
			if(Selectable("Orbiter", false)){
				choice = (int)MODULE_TYPE_ORBITER;
			}
			if(Selectable("Circles", false)){
				choice = (int)MODULE_TYPE_CIRCLES;
			}
			if(Selectable("Image", false)){
				choice = (int)MODULE_TYPE_IMAGE;
			}
			if(Selectable("Particles", false)){
				choice = (int)MODULE_TYPE_PARTICLES;
			}
			
			EndCombo();
		}
		
		SameLine();
		if(Button("Add Module"))
		{
			if(current_module_id > 0)
			{
				current_module_id -= 1;
			}else{
				current_module_id = 0;
			}
				
			std::shared_ptr<Module> module_ptr = add_module((MODULE_TYPE)choice, current_module_id);
			
			std::shared_ptr<ActionAddModule> 
			action = std::make_shared<ActionAddModule>(					
					current_module_id,
					(MODULE_TYPE)choice, 
					module_ptr,
					add_module_ptr, 
					remove_module_ptr
			);
			
			
			register_action(action);	
			
		}
		SameLine();
		if( renderer.m_modules.size() > 0)
		{
			if(Button("Remove"))
			{
				if( current_module_id != -1)
				{
					std::shared_ptr<ActionRemoveModule> action = std::make_shared<ActionRemoveModule>(
																							renderer.m_modules[current_module_id]->getType(), 
																							renderer.m_modules[current_module_id], 
																							current_module_id, 
																							add_module_ptr, 
																							remove_module_ptr
																							);
					
					
					register_action(action);						

					
					if(renderer.m_modules.size() == 0)
					{
						current_module_id = -1;
					}else if(current_module_id > (int)(renderer.m_modules.size() -1)){
						current_module_id = renderer.m_modules.size() - 1;
					}
					
				}
					
			}
			
			ImGui::SameLine();
			
			if(Button("Duplicate"))
			{
				if( current_module_id != -1)
				{
					duplicate_module(renderer.m_modules[current_module_id]);
					std::shared_ptr<ActionDuplicateModule> action = std::make_shared<ActionDuplicateModule>(
																							renderer.m_modules[current_module_id], 
																							current_module_id, 
																							add_module_ptr, 
																							remove_module_ptr
																							);
					
					
					register_action(action);						

					
					if(renderer.m_modules.size() == 0)
					{
						current_module_id = -1;
					}else if(current_module_id > (int)(renderer.m_modules.size() -1)){
						current_module_id = renderer.m_modules.size() - 1;
					}					
				}
				
			}			
					
		}		
		
		int inc = 0;
		
		for(auto module : renderer.m_modules)
		{		
			
			ImGui::PushID(module.get());

			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(ImGui::GetContentRegionAvailWidth() + p.x, p.y + ImGui::GetFontSize()), IM_COL32(20,20,40,255))	;				
			if( Selectable("##title", inc == current_module_id, 0, ImVec2( ImGui::GetContentRegionAvailWidth(), GetFontSize() * 1.0 )))
			{

				current_module_id = inc;
			}
			
			
			
	
			
			ImGuiDragDropFlags src_flags = 0;
			
			if(BeginDragDropSource(src_flags))
			{
				if( !(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip)){
								
				}				
				SetDragDropPayload("DRAGDROP", &inc, sizeof(int));
				EndDragDropSource();
			}
			
			if(BeginDragDropTarget())
			{
				ImGuiDragDropFlags target_flags = 0;
				//~ target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
				
				if( const ImGuiPayload * payload = AcceptDragDropPayload("DRAGDROP", target_flags) )
				{
					//~ printf("move from %d \n", *(const int*)payload->Data);
					//~ printf("move to %d \n", inc);
					
					std::shared_ptr<ActionMoveModule> action = std::make_shared<ActionMoveModule>(
						*(const int*)payload->Data , 
						inc , 
						[](){
						
					});
					action->m_move_function = move_module;
					//~ actions.insert(actions.begin(), action );	
					register_action(action);				
					move_module(*(const int*)payload->Data, inc);
				}
				
				EndDragDropTarget();
			}	

            ImGui::SetCursorPosY(GetCursorPosY() - GetFontSize() * 1.0 - 5.0);            
            
            ImGui::Text(module->p_name->getValue().c_str());
            
            ImGui::SetCursorPosY(GetCursorPosY() + 5.0);
            
            
			UI_widget(module->p_opacity);
			
			
			ImGui::PopID();			

			
			inc++;
		}
		
		ImGui::End();
	}
	
}

void draw_param_layout(ParamLayout& layout)
{
	ImGui::Text(layout.getName());
	ImGui::Separator();
	for(auto param : layout.params){
		UI_widget(param);
		
	}
}

//// GLFW callbacks
void live_window_size_callback(GLFWwindow* window, int width, int height)
{
	printf("width : %d -- height %d \n", width, height);
	live_w_width = width;
	live_w_height = height;
	
	//~ init_viewport_FBO();
	renderer.initFBO(live_w_width, live_w_height);
}

void execute_widget_release(std::shared_ptr<BaseParam> param, std::shared_ptr<ReleaseData> data, std::shared_ptr<BaseParam> parent_param = nullptr)
{
	//~ printf("param name : %s\n", param->getName());
	Param<float> * p_float = nullptr;
	ReleaseDataFloat * data_float = nullptr;
	
	Param<int> * p_int = nullptr;
	ReleaseDataInt * data_int = nullptr;	
	
	ParamColor3 * p_color3 = nullptr;
	ReleaseDataColor3 * data_color3 = nullptr;		
	
	Param<SignalRange> * p_signal_range = nullptr;
	ReleaseDataSignalRange * data_signal_range = nullptr;	
	
	if((p_float = dynamic_cast<Param<float> *>(param.get())) && (data_float = dynamic_cast<ReleaseDataFloat * >(data.get())))
	{		
		std::shared_ptr<ActionParamChange<float> > action = std::make_shared<ActionParamChange<float> >(p_float, data_float->old_val, data_float->new_val, data_float->callback);
		//~ actions.insert(actions.begin(), action );
		register_action(action);
				
	}else if((p_int = dynamic_cast<Param<int> *>(param.get())) && (data_int = dynamic_cast<ReleaseDataInt * >(data.get())))
	{		
		std::shared_ptr<ActionParamChange<int> > action = std::make_shared<ActionParamChange<int> >(p_int, data_int->old_val, data_int->new_val, data_int->callback);
		//~ actions.insert(actions.begin(), action );	
		register_action(action);
			
	}else if((p_color3 = dynamic_cast<ParamColor3 *>(param.get())) && (data_color3 = dynamic_cast<ReleaseDataColor3 * >(data.get())))
	{
		std::shared_ptr<ActionParamColor3Change > action = std::make_shared<ActionParamColor3Change >(p_color3, data_color3->old_val, data_color3->new_val, data_color3->callback);
		//~ actions.insert(actions.begin(), action );		
		register_action(action);
	}else if((p_signal_range = dynamic_cast<Param<SignalRange> *>(param.get())) && (data_signal_range = dynamic_cast<ReleaseDataSignalRange * >(data.get())))
	{
		if(parent_param != nullptr)
		{			
			std::shared_ptr<ActionParamSignalRangeChange> action = std::make_shared<ActionParamSignalRangeChange >(parent_param.get(), data_signal_range->old_val, data_signal_range->new_val, data_signal_range->callback);
			//~ actions.insert(actions.begin(), action );	
			register_action(action);	
		}
	}
} 

void UI_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
		//~ printf("\tleft button press\n");
	}
	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		if( is_param_clicked)
		{	
			execute_widget_release(active_param, current_param_data, current_param_data->parent_param);

			is_param_clicked = false;
		}
	}
}

void UI_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//~ printf("scancode : %d, mods : %d\n", scancode, mods );
    if (scancode == 25 /* z key */ && action == GLFW_PRESS && mods == 2 /* ctrl */)
    {

			action_undo();
	}else if (scancode == 29 /* y key */ && action == GLFW_PRESS && mods == 2 /* ctrl */)
	{

			action_redo();
	}
}
/////////

void make_sound(const char * wav_path)
{	
	wave_reader.read(wav_path);
	
	while(true)
	{
		wave_reader.play( &sound_player_cmd, &sound_player_mode, &sine_wave_frequency, true);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void compute_fft_maximums(float threshold, float exponent)
{
	//~ double max = abs(fft_out[0][0]);
	int num_bands = NUM_BANDS;
	int fft_num_samples = 256;
	int num_per_band = fft_num_samples / num_bands;
	for(size_t i=0; i < NUM_BANDS/2; i++)
	{			
		double accum = 0.0;
		for (int j = 0; j < num_per_band; j++)
		{
			double fft_val = (abs(fft_out[(i*num_per_band)+j][0])*2.0)/fft_num_samples;
			accum += fft_val;
		}
		
		accum /= (double)num_per_band;		
		accum *= (double)(i+1)*(i != 0 ? exponent : 1.0);
		
		
		
		if(accum < (double)threshold){
			accum = 0.0;
		}
		
		fft_values[i] = (float)accum;
		
		if( accum > fft_maximums[i])
		{
			fft_maximums[i] = (float)(accum);
			
		}else{
			fft_maximums[i] *= 1.0f - 0.005 * (float)timer->getDeltaMillis();
		}
	}	
}

void display_fft_values()
{
	ImGui::Text("FFT :");
	ImGui::BeginChild("fft_child", ImVec2(ImGui::GetContentRegionAvail().x,200.0f) , true);
	
	//~ static float threshold  = 0.1;
	//~ static float exponent  = 1.5;
	if( ImGui::SliderFloat("##Threshold", &fft_threshold, 0.0, 1.0, "Threshold = %.3f"))
	{
		
	}
	
	if( ImGui::SliderFloat("##Exponent", &fft_exponent, 0.0, 10.0, "Exponent = %.3f"))
	{
		
	}		
	
	//~ double max = abs(fft_out[0][0]);
	int num_bands = NUM_BANDS;
	int fft_num_samples = 256;
	int num_per_band = fft_num_samples / num_bands;
	for(size_t i=0; i < NUM_BANDS/2; i++)
	{		
		double height;
		height = fft_values[i] * 200.0f;
		
		
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(ImGui::GetCursorScreenPos().x + i*2*num_per_band, ImGui::GetCursorScreenPos().y),
			ImVec2(
					ImGui::GetCursorScreenPos().x + i*2*(float)num_per_band + num_per_band*2, 
					ImGui::GetCursorScreenPos().y  + height
			), 						
			IM_COL32(255,255,255,30)
		);
	}
	
	for (size_t i = 0; i < NUM_BANDS/2; i++)
	{
		double height = (double)fft_maximums[i] * 200.0;
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(
				ImGui::GetCursorScreenPos().x + i*2*num_per_band, 
				ImGui::GetCursorScreenPos().y + height - 2
			),
			ImVec2(
				ImGui::GetCursorScreenPos().x + i*2*(float)num_per_band + num_per_band*2, 
				ImGui::GetCursorScreenPos().y  + height
			), 						
			IM_COL32(255,255,255,150)
		);			
	}
	
	
	ImGui::EndChild();	
}

void sound_dialog()
{
	if(ImGui::Begin("Sound"), true)
	{
		const char * modes[2] = {"Wave file Player", "Sine Wave Generator"};
		static unsigned int choice = 0;
		
		ImGui::Text("Mode");
		ImGui::SameLine();
		
		ImGui::PushItemWidth(-1);
		if( ImGui::BeginCombo("##Mode", modes[choice]))
		{
			for(size_t i = 0; i < 2; i++)
			{
				if(ImGui::Selectable(modes[i], choice == i))
				{
					choice = i;
				}
				
			}

			ImGui::EndCombo();
		}
		
		if(ImGui::Button("Change")){
			sound_player_mode = (SOUND_PLAYER_MODE)choice;
		}
		
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		
		if( sound_player_mode == SOUND_PLAYER_MODE_FILE)
		{
			UI_widget(sound_player_wave_file_path_param);
		}
		else if(sound_player_mode == SOUND_PLAYER_MODE_SINE_WAVE)
		{
			static float freq = 440.0;
			ImGui::Text("Frequency :");
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			if(ImGui::InputFloat("##freq", &freq ))
			{
				sine_wave_frequency = freq;
				printf("%.3f\n",freq);
			}
		}
		
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
				
		if(ImGui::Button("Play"))
		{
			if( sound_player_cmd == SOUND_PLAYER_CMD_STOP)
			{
				wave_reader.read(sound_player_wave_file_path_param->getValue().c_str());
				sound_player_cmd = SOUND_PLAYER_CMD_PLAY;				
			}else
			{
				sound_player_cmd = SOUND_PLAYER_CMD_PLAY;
			}			
		}
		
		ImGui::SameLine();
		
		if(ImGui::Button("Pause"))
		{
			sound_player_cmd = SOUND_PLAYER_CMD_PAUSE;
		}
		
		ImGui::SameLine();
		
		if(ImGui::Button("Stop"))
		{
			sound_player_cmd = SOUND_PLAYER_CMD_STOP;
		}
		
		if(ImGui::CollapsingHeader("Wave form :", 0))
		{
			ImGui::BeginChild("child", ImVec2(ImGui::GetContentRegionAvail().x,200.0f) , true);
			
			for(size_t i=0; i < 512; i++)
			{
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImVec2(ImGui::GetCursorScreenPos().x + i, ImGui::GetCursorScreenPos().y + (ImGui::GetContentRegionAvail().y / 2.0)),
					ImVec2(
							ImGui::GetCursorScreenPos().x + i + 1, 
							ImGui::GetCursorScreenPos().y + (ImGui::GetContentRegionAvail().y / 2.0) + sound_buffer[i*2]  * 100.0f
					), 
					IM_COL32(255,255,255,150)
				);
			}
			
			ImGui::EndChild();
		}
		
		if(ImGui::CollapsingHeader("Frequencies :", ImGuiTreeNodeFlags_DefaultOpen))
		{		
			display_fft_values();
		}
						
		ImGui::End();
	}
}

void parameter_signal_dialog()
{
	
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking;
	
	if(ImGui::Begin("Signal", &signal_range_dialog_opened, flags)) //, &signal_range_dialog_opened, flags))
	{
		for( auto module : renderer.m_modules)
		{			
			bool has_signal = false;
			for(auto param : module->param_layout.params)
			{			
				if(param->getUseSignalRange())
				{
					has_signal = true;
					break;
				}
			}
			
			if( has_signal)
			{
				if(ImGui::CollapsingHeader(module->getName().c_str()))
				{
					for(auto param : module->param_layout.params)
					{
						if( param->getUseSignalRange())
						{
							//~ ImGui::Text(param->getName());
							UI_widget(module->p_signal_range, param);
						}
					}	
				}
			}
		}
	}
	
	ImGui::End();
}


int main(int argc, char** argv)
{
	
	//~ Log::init();
	//~ Log::getLogger()->warn("warning");
	//~ VJ_LOG_TRACE("Simple message ");
	//~ VJ_LOG_INFO("Info ");
	//~ VJ_LOG_WARN("Warning");
	//~ VJ_LOG_ERROR("Error in main ");

	
	memset(fft_maximums, 0.0, sizeof(float) * NUM_BANDS);
	FFT fft;
	
	
	for(size_t i=0; i< 512; i++){
		sound_buffer[i] = 0.5;
	}
	

	//sound thread
	std::string str_wav_path = "/home/pi/Downloads/guitar_riff.wav";
	WAV_PATH = (char *)(str_wav_path.c_str());
	std::thread t(make_sound, WAV_PATH);
	
	//noise thread
	std::thread noise_thread(noise_threaded, &noise_trigger_update);
	
	Client client;
	int res;
	client.send_message("hello server ...", res);
	client.send_message("whos the boss now ...",res);

	
	sound_player_wave_file_path_param->setName("Sound File");
	sound_player_wave_file_path_param->setValue(std::string(WAV_PATH));
	sound_player_wave_file_path_param->setCallback( []()
		{
			strcpy(WAV_PATH, sound_player_wave_file_path_param->getValue().c_str());
		});

	
	std::shared_ptr<ParamButton > play_button = std::make_shared<ParamButton >();
	play_button->setName("send PLAY");
	play_button->setCallback( [&client, &res](){
		client.send_message("PLAY", res);
	});
	player_layout.addParam(play_button);		
	
	std::shared_ptr<ParamButton > stop_button = std::make_shared<ParamButton >();
	stop_button->setName("send STOP");
	stop_button->setCallback( [&client, &res](){
		client.send_message("STOP", res);
	});
	player_layout.addParam(stop_button);			
	

    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);		
	
	if( !glfwInit()){
		printf("glfw init error\n");
		return -1;
	}
	
	
	ui_window = glfwCreateWindow(w_width, w_height,"UI Window", NULL, NULL);
	live_window = glfwCreateWindow(live_w_width, live_w_height,"other window", NULL, ui_window);
	
	if( !live_window){
		printf("glfw live_window error ??\n");
		glfwTerminate();
	}
	if( !ui_window){
		printf("glfw window error\n");
		glfwTerminate();
	}
	
	
	glfwSetWindowSizeCallback(live_window, live_window_size_callback);
	
	glfwSetMouseButtonCallback(ui_window, UI_mouse_button_callback);
	glfwSetKeyCallback(ui_window, UI_key_callback);
	glfwMakeContextCurrent(ui_window);
	
	
	glewInit();
	
	

	
	printf("GL_VERSION : %s\n", glGetString(GL_VERSION));
	printf("GL_SHADING_LANGUAGE_VERSION : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	

	
	
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //~ io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //~ io.ConfigViewportsNoAutoMerge = true;
    //~ io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    io.Fonts->AddFontDefault(); // load default font 
    io.ConfigDockingWithShift = true;
    // add second font
    small_font = io.Fonts->AddFontFromFileTTF("../src/fonts/ProggyTiny.ttf", 10.0f);
    
    
    
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(ui_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);    
	
	

	

	
	glEnable(GL_TEXTURE_2D);

	
	renderer.initTexture();
	renderer.initFBO(live_w_width, live_w_height);


	timer->start();
	int cur_time = 0;
	int old_time = 0;
	while (!glfwWindowShouldClose(live_window)){
		
		if( noise_trigger_update)
		{
			update_noise_texture();
			noise_trigger_update = false;
		}
			
		double fft_temp[512];
		for (size_t i = 0; i < 512; i++)
		{
			fft_temp[i] = (double)sound_buffer[i*2];
		}
		
		fft_out = fft.execute_plan(fft_temp);
		compute_fft_maximums(fft_threshold, fft_exponent);
		
		timer->update();
		
		cur_time = timer->getMillis();
		
		old_time = cur_time;
		
		glfwMakeContextCurrent(ui_window);
		
		
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		renderer.updateModules();
		renderer.render();

		
		// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
			//~ static ImGuiID dockspaceID = 0;
			
			
			bool active = true;
			
			if( b_explorer_opened )
			{
				explorerDialog_V2("/home/pi/Downloads");
			}
			
			if (active)
			{
				// Declare Central dockspace

				ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_None|ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);
			}

			
			
			parameter_signal_dialog();
			module_list_dialog();
			actions_dialog();
			sound_dialog();
			noise_dialog();
			
			if (ImGui::Begin("Main Menu",  &active))
			{
				
				if(ImGui::BeginMenu("File"))
				{
					if(ImGui::MenuItem("load"))
					{
						printf("load ????\n");
						b_explorer_opened = true;
						current_explorer_callback = [](){
							loadFromFile();
						};
					}
					if(ImGui::MenuItem("save"))
					{
						printf("save ????\n");
						b_explorer_opened = true;
						current_explorer_callback = [](){
							saveToFile();
						};
					}					
					
					if(ImGui::MenuItem("exit"))
					{
										
					}
					
					ImGui::EndMenu();
				}
					
				if(ImGui::BeginMenu("Edit"))
				{
					if(ImGui::MenuItem("Undo"))
					{
						action_undo();
					}
					
					if(ImGui::MenuItem("Redo"))
					{
						action_redo();
					}					
					
					ImGui::EndMenu();
				}	
				ImGui::End(); 
			}
			
			 
			if (ImGui::Begin("Viewport"), &active)
			{
				int avail_width = ImGui::GetContentRegionAvail().x;				
				
				float _ratio = (float)live_w_width / (float)live_w_height;
				ImGui::Image((void*)(uintptr_t)renderer.m_texture, ImVec2(avail_width,(int)((float)avail_width / _ratio)),ImVec2(0,1), ImVec2(1,0));
				ImGui::End(); 
			}
			
			if (ImGui::Begin("Noise Viewer"), &active)
			{
				//~ int avail_width = ImGui::GetContentRegionAvail().x;				
				
				
				//~ ImGui::Image((void*)(uintptr_t)noise_texture, ImVec2(avail_width,(int)((float)avail_width)),ImVec2(0,1), ImVec2(1,0));
				ImGui::End(); 
			}			
			
			
			if (ImGui::Begin("Player"), &active)
			{
				draw_param_layout(player_layout);
				ImGui::End(); 
			}
			
			
			if (ImGui::Begin("Module Params"), &active)
			{
				
				if( current_module_id != -1)
				{
					draw_param_layout(renderer.m_modules[current_module_id]->param_layout);
				}
					
				ImGui::End(); 

			}
						
        
        ImGui::Render();	
		
		//~ glClearColor(0.0,0.0,0.0,1.0);
		//~ glClear(GL_COLOR_BUFFER_BIT);
		

		
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        //~ ImGui::CreateContext();
        
        //~ ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }		
		
		
		glfwPollEvents();
		glfwSwapBuffers(ui_window);
		counter++;
		
		
		glfwMakeContextCurrent(live_window);
		
		//~ float aspect = (float)live_w_width / (float)live_w_height;
		glViewport(0, 0, live_w_width, live_w_height);
		
	
		
		//~ glClearColor(0.0,0.0,1.0,0.1);		
		//~ glClear(GL_COLOR_BUFFER_BIT);
		
		renderer.displayScreen();	
		
		glfwPollEvents();
		glfwSwapBuffers(live_window);
		//~ update_otherWindow();
	}
	
		
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
	
	
	//~ glfwDestroyWindow(ui_window);
	//~ if(live_window != nullptr)
	//~ {
		//~ t.detach();
		//~ glfwDestroyWindow(live_window);
	//~ }
	
	glfwTerminate();
	exit(0);
	return 0;
}
