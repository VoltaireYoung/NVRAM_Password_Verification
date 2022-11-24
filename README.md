# Setup Password Verification

## Description
  A tool for verifying the administrator/user password stored in BIOS NVRAM. <br>
  This tool will automatically identify the hash algorithm used by the current BIOS envirnmemt where it located.


## Available Hash Algorithm
<ol>
	<li> OTP (One Time Pad) </li>
	<li> SHA-256 </li>
</ol>


## How to Use
<ol>
	<li>Instruction List</li>
		<ol>
			<li> -help <br>
				  Print out all available instructions.
			</li>
			<li> -nvlock_status <br>
				   Fetch current NvLock Status.
			</li>
			<li> -unlock + &lt;password&gt; <br>
					 Unlock variables access protection via NvLock. <br>
					 (Unlock Only!)
			</li>
			<li> -change_password + &lt;password&gt; <br>
			     Change administrator's password if Nvlock already be unlocked. <br>
			</li>
		</ol>
</ol>

## Resource
<ol>
	<li> [AMI Aptio 5.x	AMISCE User Guide](https://cp.ami.com/Shared%20Documents/Aptio%20V/Utilities/AMI%20Setup%20Control%20Environment%20(SCE)/AMI_Aptio_5.x_SCE_User_Guide_NDA.pdf) </li>
</ol>

## Reference
<ol>
	<li> [EDK2](https://github.com/tianocore/edk2) </li>
	<li> [AMI Aptio 5.x	AMISCE User Guide](https://cp.ami.com/Shared%20Documents/Aptio%20V/Utilities/AMI%20Setup%20Control%20Environment%20(SCE)/AMI_Aptio_5.x_SCE_User_Guide_NDA.pdf) </li>
	<li> [OpenSSL](https://github.com/openssl) </li>
</ol>

***

## Requirement
<ol>
	<li> [EDK2](https://github.com/tianocore/edk2)
		<ol>
			<li> MdePkg </li>
			<li> MdeModulePkg </li>
			<li> CryptoPkg </li>
			<li> ShellPkg</li>
		</ol>
	</li>
	<li> [Visual Studio 2017](https://visualstudio.microsoft.com/zh-hant/vs/older-downloads/) </li>
	<li> [Python 2.7](https://www.python.org/downloads/)</li>
</ol>

## Authors and acknowledgment
Author: Voltaire Chen (Voltaire_Chen@pegatroncorp.com)

## Support
Voltaire Chen (Voltaire_Chen@pegatroncorp.com)