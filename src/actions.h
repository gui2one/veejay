#ifndef ACTIONS_H
#define ACTIONS_H

#include "pch.h"

#include "param.h"
#include "core/modules/module.h"



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

	ActionParamChange(Param<T> * param, T _old_value, T _new_value, std::function<void()> callback) : Action(){
		setName("Param Change");
		m_param = param;
		m_callback = callback;
		m_old_value = _old_value;

		m_new_value = _new_value;

		
		
		
		param->setValue(m_new_value);
		
		m_callback();
	};
	void redo()override
	{	
		//~ std::cout << "New Value : " << m_new_value << std::endl;
		//~ std::cout << "Old Value : " << m_old_value << std::endl;
		m_param->setValue(m_new_value);
		m_callback();
	};
	
	void undo()override
	{
		m_param->setValue(m_old_value);
		m_callback();
	};
	
	Param<T> * m_param;
	T m_old_value, m_new_value;

	std::function<void()> m_callback;
		
private:
};


class ActionParamUseSignalChange : public Action
{
public :
	ActionParamUseSignalChange(BaseParam * param, bool old_value, bool new_value, std::function<void()> callback = [](){}) : Action()
	{
		setName("Use Signal Change");
		m_param = param;
		m_callback = callback;
		m_old_value = old_value;
		m_new_value = new_value;
		callback();
	}
	
	void redo()override{
		m_param->setUseSignalRange(m_new_value);
		m_callback();	
	};
	
	void undo()override{
		m_param->setUseSignalRange(m_old_value);
		m_callback();
	} ;	
private:
	BaseParam * m_param;
	bool m_old_value, m_new_value;
	std::function<void()> m_callback;	
};


class ActionParamSignalRangeChange : public Action
{
public:

	ActionParamSignalRangeChange(BaseParam * param, SignalRange _old_value, SignalRange _new_value, std::function<void()> callback) : Action(){
		setName("Signal Range Change");
		m_param = param;
		m_callback = callback;
		m_old_value = _old_value;

		m_new_value = _new_value;

		
		
		
		param->setSignalRange(m_new_value);
		
		callback();
	};
	void redo()override{
		m_param->setSignalRange(m_new_value);
		m_callback();		
	};
	
	void undo()override{
		m_param->setSignalRange(m_old_value);
		m_callback();
	} ;
	
	BaseParam * m_param;
	SignalRange m_old_value, m_new_value;

	std::function<void()> m_callback;
		
private:
};

class ActionParamMenuChange : public Action
{
public:
	ActionParamMenuChange(ParamMenu * param, int old, int val, std::function<void()> callback) : Action()
	{
		setName("Param Menu Change");
		m_param = param;
		m_callback = callback;
		m_old_value = old;
		m_new_value = val;
		
		m_param->setValue(val);
		
		m_callback();		
	}
	
	void redo()override{
		m_param->setValue(m_new_value);
		m_callback();	
	};
	
	void undo()override{
		m_param->setValue(m_old_value);
		m_callback();
	} ;
	
	ParamMenu * m_param;
	int m_old_value, m_new_value;
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
		m_old_value = old;
		m_new_value = val;
		//~ printf("old value stored in %s :  %.3f, %.3f, %.3f\n", param->getName(), old.x, old.y, old.z);
		//~ printf("New value stored in %s :  %.3f, %.3f, %.3f\n", param->getName(), new_value.x, new_value.y, new_value.z);
		m_param->color = val;
		
		m_callback();		
	}
	
	void redo()override{
		m_param->setValue(m_new_value);
		m_callback();		
	};
	
	void undo()override{
		m_param->setValue(m_old_value);
		m_callback();
	} ;
	
	ParamColor3 * m_param;
	glm::vec3 m_old_value, m_new_value;
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
		m_old_value = old;
		m_new_value = val;
		m_callback = callback;
		m_param->setValue(val);
		m_callback();
	}
	
	void redo()override{
		m_param->setValue(m_new_value);
		m_callback();		
	}
	
	void undo()override
	{
		m_param->setValue(m_old_value);
		m_callback();
	}
	
private:
	ParamFilePath * m_param;
	std::string m_old_value;
	std::string m_new_value;
	
	std::function<void()> m_callback;
};

class ActionAddModule : public Action
{
public : 
	ActionAddModule( 
		MODULE_TYPE type,
		std::shared_ptr<Module> module_ptr,
		std::function< std::shared_ptr<Module > (MODULE_TYPE, unsigned int) >  add_func, 
		std::function<void(unsigned int)> remove_func, 
		std::function<void()> callback = [](){}
	) : Action()
	{
		setName("Add Module");
		m_type = type;
		m_module_ptr = module_ptr;
		m_add_module_function = add_func;
		m_remove_module_function = remove_func;
	}
	
	void redo()override{
		printf("---- REDO Add Module\n");
		m_add_module_function(m_type, 0);
	}
	
	void undo()override{
		printf("---- UNDO Add Module\n");
		m_remove_module_function(0);
	}
	
	MODULE_TYPE m_type;
	std::shared_ptr<Module> m_module_ptr;
	std::function< std::shared_ptr<Module > (MODULE_TYPE, unsigned int) > m_add_module_function;
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

