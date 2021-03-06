#include "qafcore.h"
#include <QDockWidget>
#include <QDebug>
#include "ObjectSystem.h"
#include "PluginSystem.h"
#include "ConfigSystem.h"
#include "UIInterface.h"
#include "Logger.h"
#include "LogModel.h"

namespace QAF
{
	QAFCore::QAFCore()
		:mMessageCallback(nullptr)
		, mUIInterface(nullptr)
		, mLogModel(new QAF::LogModel(this))
	{
		mLogModel->setHeaders(QStringList() << LStr("类型") << LStr("时间") << LStr("内容"));
	}

	QAFCore::~QAFCore()
	{
		foreach(int key, mSystems.keys()){
			AbstractSystem* system = mSystems.value(key, NULL);
			if (system){
				QString msg;
				if (key > ST_NONE && key < ST_CORE){
					msg = QString("%1%2").arg(LStr("正在卸载核心模块：")).arg(system->name());
				} else if (key > ST_CORE){
					msg = QString("%1%2").arg(LStr("正在卸载扩展模块：")).arg(system->name());
				}

				qDebug() << msg;
				system->uninstall();
			}
		}
		
		qDeleteAll(mSystems);

		//取消消息处理
		qInstallMessageHandler(0);
	}

	void QAFCore::initialize(MessageCallback fun)
	{
		static bool init = false;
		if (!init)
		{
			init = true;
			qInstallMessageHandler(Logger::messageHandler);
			mMessageCallback = fun;
			if (mMessageCallback)
				(*mMessageCallback)(LStr("正在初始化..."));

			initCore(); //初始化核心层
			initExtent(); //初始化扩展层
		}
	}

	void QAFCore::initCore()
	{
		addSystem(new ObjectSystem(ST_OBJECT,this));
		addSystem(new ConfigSystem(ST_CONFIG,this));

		foreach(int key, mSystems.keys()){
			if (key > ST_NONE && key < ST_CORE){
				AbstractSystem* system = mSystems.value(key, NULL);
				if (system){
					QString msg = QString("%1%2").arg(LStr("正在加载核心模块：")).arg(system->name());
					qDebug() << msg;
					if (mMessageCallback){
						(*mMessageCallback)(msg);
					}
					
					system->install();
				}
			}
		}
	}

	void QAFCore::initExtent()
	{
		addSystem(new PluginSystem(ST_PLUGIN,this));

		foreach(int key, mSystems.keys()){
			if (key > ST_CORE){
				AbstractSystem* system = mSystems.value(key, NULL);
				if (system){
					QString msg = QString("%1%2").arg(LStr("正在加载扩展模块：")).arg(system->name());
					qDebug() << msg;
					if (mMessageCallback){
						(*mMessageCallback)(msg);
					}

					system->install();
				}
			}
		}
	}

	void QAFCore::addSystem(AbstractSystem* as)
	{
		if (!as)
			return;

		int id = as->systemId();
		if (!mSystems.contains(id))
		{
			mSystems.insert(id, as);
		}
	}

	void QAFCore::removeSystem(SystemType type)
	{
		if (mSystems.contains(type))
		{
			AbstractSystem* as = mSystems.take(type);
			if (as)
			{
				as->uninstall();
				delete as;
			}
		}
	}

	AbstractSystem* QAFCore::getSystem(int type) const
	{
		return mSystems.value(type, nullptr);
	}

	void QAFCore::setUIInterface(UIInterface* ui)
	{
		if (ui && !mUIInterface)
			mUIInterface = ui;
	}

	UIInterface* QAFCore::getUIInterface() const
	{
		return mUIInterface;
	}

	ConfigSystem* QAFCore::getConfigSystem() const
	{
		return static_cast<ConfigSystem*>(getSystem(ST_CONFIG));
	}

	ObjectSystem* QAFCore::getObjectSystem(int sysId) const
	{
		return static_cast<ObjectSystem*>(getSystem(sysId));
	}

	PluginSystem* QAFCore::getPluginSystem() const
	{
		return static_cast<PluginSystem*>(getSystem(ST_PLUGIN));
	}

	LogModel* QAFCore::getLogModel()
	{
		return mLogModel;
	}

}