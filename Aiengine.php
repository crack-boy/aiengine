<?php
namespace engine\aiengine;
class Aiengine{
	public $aiengine=null;
	public $cfg;

	
	public function __construct(){
		$so_file=dirname(__FILE__).'/m_aiengine.so';
		$provision_file=dirname(__FILE__).'/aiengine.provision';
		$config=require dirname(__FILE__).'/config.php';
		
		if (!file_exists($so_file)){
			die('file m_aiengine.so is not exists!');
		}
		$module=swoole_load_module($so_file);
		if (!$module){
			die('load module failed!');
		}
		$this->aiengine=$module;
		$cfg=array(
			'appKey'=>$config['appKey'],
			'secretKey'=>$config['secretKey'],
			'provision'=>$provision_file,
			'cloud'=>array(
					'enable'=>1,
					'server'=>$config['server'],
			)
		);
		$this->cfg=json_encode($cfg);
		
	}
	public function scoreResule($refText,$audio,$rank,$qType,$coreType='en.sent.score',$sampleRate=16000){
		$fileInfo=pathinfo($audio);
		$parmas=array(
				'coreProvideType'=>'cloud',
				'app'=>array(
						"userId"=>'test'
		
				),
				'audio'=>array(
						'audioType'=>$fileInfo['extension'],
						'channel'=>1,
						'sampleBytes'=>2,
						'sampleRate'=>intval($sampleRate),
				),
				'request'=>array(
						//["en.sent.score", "en.word.score","en.sent.rec","cn.asr.rec","cn.sent.score","cn.word.score","cn.sent.syn","en.pred. exam","cn.sent.rec","en.syn"]
						'coreType'=>$coreType,
						'refText'=>$refText,
						'rank'=>floatval($rank),
						'precision'=>0.5,
				),
				
		);
		$parmas=json_encode($parmas);
		return $this->aiengine->m_aiengine($this->cfg,$parmas,$audio,$fileInfo['extension']);
		
	}
	
	
	
	
	
}