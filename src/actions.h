#ifndef ACTIONS_H
#define ACTIONS_H

#include "pch.h"

#include "param.h"
#include "core/modules/module.h"

/*
template <typename T>
struct ParamSnapshot{

	std::pair< Param<T>, T > old_value, new_value;
	
	void printValues(){
		printf("what do I do now ?\n");
	}
};
*/

class Action{
public:
	Action(){}
	virtual void redo(){};
	virtual void undo(){};
	
	inline const char * getName(){
		return m_name;
	}
	
	inline void setName(const char * name)
	{
		m_name = name;
	}
	//~ std::pair< BaseParam *, float > old_value, new_value;
	
private:

	const char * m_name = "Action Default";
};

template<typename T>
class ActionParamChange : public Action
{
public:

	ActionParamChange(Param<T> * param, T old, T val, std::function<void()> callback) : Action(){
		setName("Param Change");
		m_param = param;
		m_callback = callback;
		old_value = old;
		new_value = val;
		//~ printf("old value stored : %s\n", param->getName().c_str());
		
		
		param->setValue(val);
		
		callback();
	};
	void redo()override{};
	
	void undo()override{
		m_param->setValue(old_value);
		m_callback();
	} ;
	
	Param<T> * m_param;
	T old_value, new_value;
	std::function<void()> m_callback;
		
private:
};



class ActionParamMenuChange : public Action
{
public:
	ActionParamMenuChange(ParamMenu * param, int old, int val, std::function<void()> callback) : Action()
	{
		setName("Param Change");
		m_param = param;
		m_callback = callback;
		old_value = old;
		new_value = val;
		printf("old value stored in %s : %d\n", param->getName(), param->getValue());
		param->setValue(val);
		
		callback();		
	}
	
	void redo()override{};
	
	void undo()override{
		m_param->setValue(old_value);
		m_callback();
	} ;
	
	ParamMenu * m_param;
	int old_value, new_value;
	std::function<void()> m_callback;	
private:
};

class ActionParamColor3Change : public Action
{
public:
	ActionParamColor3Change(ParamColor3 * param, glm::vec3 old, glm::vec3 val, std::function<void()> callback) : Action()
	{
		setName("Color3 action");
		m_param = param;
		m_callback = callback;
		old_value = old;
		new_value = val;
		printf("old value stored in %s :  %.3f, %.3f, %.3f\n", param->getName(), old.x, old.y, old.z);
		printf("New value stored in %s :  %.3f, %.3f, %.3f\n", param->getName(), new_value.x, new_value.y, new_value.z);
		param->color = val;
		
		callback();		
	}
	
	void redo()override{};
	
	void undo()override{
		m_param->setValue(old_value);
		m_callback();
	} ;
	
	ParamColor3 * m_param;
	glm::vec3 old_value, new_value;
	std::function<void()> m_callback;	
private:
};

class ActionParamFilePathChange : public Action
{
public :
	ActionParamFilePathChange(ParamFilePath * param,  std::string old, std::string val, std::function<void()> callback = [](){}) : Action()
	{
		setName("File Path action");
		m_param = param;
		old_value = old;
		new_value = val;
		m_param->setValue(val);
		callback();
	}
	
	void redo()override{}
	
	void undo()override
	{
		m_param->setValue(old_value);
	}
	
private:
	ParamFilePath * m_param;
	std::string old_value;
	std::string new_value;
};

class ActionAddModule : public Action
{
public : 
	ActionAddModule( 
		MODULE_TYPE type, 
		std::function<void(MODULE_TYPE, unsigned int)> add_func, 
		std::function<void(unsigned int)> remove_func, 
		std::function<void()> callback = [](){}
	) : Action()
	{
		setName("Add Module");
		m_add_module_function = add_func;
		m_remove_module_function = remove_func;
	}
	
	void redo()override{}
	
	void undo()override{
		m_remove_module_function(0);
	}
	
	std::function<void(MODULE_TYPE, unsigned int)> m_add_module_function;
	std::function<void(unsigned int)> m_remove_module_function;
private:


};

class ActionRemoveModule : public Action
{
public : 
	ActionRemoveModule( 
		MODULE_TYPE type, 
		std::shared_ptr<Module> module_ptr, 
		int module_id, 
		std::function<void(std::shared_ptr<Module>, unsigned int )> add_func, 
		std::function<void(unsigned int)> remove_func, 
		std::function<void()> callback = [](){}
		
	) : Action()
	{
		setName("Remove Module");
		m_add_module_function = add_func;
		m_remove_module_function = remove_func;
		
		m_type = type;
		m_module_id = module_id;
		m_module_ptr = module_ptr;
	}
	
	void redo()override{}
	
	void undo()override{
		m_add_module_function(m_module_ptr, m_module_id);
	}
	
	std::function<void(std::shared_ptr<Module>, unsigned int)> m_add_module_function;
	std::function<void(unsigned int)> m_remove_module_function;
	
	MODULE_TYPE m_type;
	int m_module_id;
	std::shared_ptr<Module> m_module_ptr;
private:


};

class ActionMoveModule : public Action
{
public: 
	ActionMoveModule(int from, int to, std::function<void()> callback) : Action()
	{
		setName("Move Module");
		m_from = from;
		m_to = to;
		callback();
	}

	void redo()override{}

	void undo()override
	{
		m_move_function(m_to, m_from);
	}

	std::function<void(int, int)> m_move_function;
	int m_to, m_from;
private:

};

	

	

#endif /* ACTIONS_H */

