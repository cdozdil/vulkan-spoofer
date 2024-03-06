// dllmain.cpp : Defines the entry point for the DLL application.
#include "dllmain.h"
#include "detours/detours.h"
#include <vulkan/vulkan_core.h>
#include <string>

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable : 4996)
#pragma comment(lib, "detours/detours.lib")

PFN_vkGetPhysicalDeviceProperties pfn_vkGetPhysicalDeviceProperties = nullptr;
PFN_vkGetPhysicalDeviceProperties2 pfn_vkGetPhysicalDeviceProperties2 = nullptr;
PFN_vkGetPhysicalDeviceProperties2KHR pfn_vkGetPhysicalDeviceProperties2KHR = nullptr;

bool pfn_vkGetPhysicalDeviceProperties_hooked = false;
bool pfn_vkGetPhysicalDeviceProperties2_hooked = false;
bool pfn_vkGetPhysicalDeviceProperties2KHR_hooked = false;

void WINAPI hkGetPhysicalDeviceProperties(VkPhysicalDevice physical_device, VkPhysicalDeviceProperties* properties)
{
	pfn_vkGetPhysicalDeviceProperties(physical_device, properties);

	std::strcpy(properties->deviceName, "NVIDIA GeForce RTX 4090");
	properties->vendorID = 0x10de;
	properties->deviceID = 0x2684;
	properties->driverVersion = VK_MAKE_API_VERSION(551, 76, 0, 0);
}

void WINAPI hkGetPhysicalDeviceProperties2(VkPhysicalDevice phys_dev, VkPhysicalDeviceProperties2* properties2)
{
	pfn_vkGetPhysicalDeviceProperties2(phys_dev, properties2);

	std::strcpy(properties2->properties.deviceName, "NVIDIA GeForce RTX 4090");
	properties2->properties.vendorID = 0x10de;
	properties2->properties.deviceID = 0x2684;
	properties2->properties.driverVersion = VK_MAKE_API_VERSION(551, 76, 0, 0);

}

void WINAPI hkGetPhysicalDeviceProperties2KHR(VkPhysicalDevice phys_dev, VkPhysicalDeviceProperties2* properties2)
{
	pfn_vkGetPhysicalDeviceProperties2KHR(phys_dev, properties2);

	std::strcpy(properties2->properties.deviceName, "NVIDIA GeForce RTX 4090");
	properties2->properties.vendorID = 0x10de;
	properties2->properties.deviceID = 0x2684;
	properties2->properties.driverVersion = VK_MAKE_API_VERSION(551, 76, 0, 0);
}

void AttachHooks()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// Detour the functions
	pfn_vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(DetourFindFunction("vulkan-1.dll", "vkGetPhysicalDeviceProperties"));
	pfn_vkGetPhysicalDeviceProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(DetourFindFunction("vulkan-1.dll", "vkGetPhysicalDeviceProperties2"));
	pfn_vkGetPhysicalDeviceProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(DetourFindFunction("vulkan-1.dll", "vkGetPhysicalDeviceProperties2KHR"));

	if (pfn_vkGetPhysicalDeviceProperties)
		pfn_vkGetPhysicalDeviceProperties_hooked = (DetourAttach(&(PVOID&)pfn_vkGetPhysicalDeviceProperties, hkGetPhysicalDeviceProperties) == 0);

	if (pfn_vkGetPhysicalDeviceProperties2)
		pfn_vkGetPhysicalDeviceProperties2_hooked = (DetourAttach(&(PVOID&)pfn_vkGetPhysicalDeviceProperties2, hkGetPhysicalDeviceProperties2) == 0);

	if (pfn_vkGetPhysicalDeviceProperties2KHR)
		pfn_vkGetPhysicalDeviceProperties2KHR_hooked = (DetourAttach(&(PVOID&)pfn_vkGetPhysicalDeviceProperties2KHR, hkGetPhysicalDeviceProperties2KHR) == 0);

	DetourTransactionCommit();
}

void DetachHooks()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (pfn_vkGetPhysicalDeviceProperties_hooked)
		DetourDetach(&(PVOID&)pfn_vkGetPhysicalDeviceProperties, hkGetPhysicalDeviceProperties);

	if (pfn_vkGetPhysicalDeviceProperties2_hooked)
		DetourDetach(&(PVOID&)pfn_vkGetPhysicalDeviceProperties2, hkGetPhysicalDeviceProperties2);

	if (pfn_vkGetPhysicalDeviceProperties2KHR_hooked)
		DetourDetach(&(PVOID&)pfn_vkGetPhysicalDeviceProperties2KHR, hkGetPhysicalDeviceProperties2KHR);

	DetourTransactionCommit();
}

void LoadVersion()
{
	auto versionHandle = LoadLibrary("version-original.dll");

	if (!versionHandle)
	{
		char dllpath[MAX_PATH];
		GetSystemDirectory(dllpath, MAX_PATH);
		strcat(dllpath, "\\version.dll");
		versionHandle = LoadLibrary(dllpath);

		if (versionHandle)
			version.LoadOriginalLibrary(versionHandle);
	}
	else
		version.LoadOriginalLibrary(versionHandle);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		LoadVersion();
		AttachHooks();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		DetachHooks();
		break;
	}

	return TRUE;
}

