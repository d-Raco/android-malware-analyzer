<p align="center"><a href="https://github.com/d-Raco/android-malware-analyzer"><img width="100%" src="img/cover.png" /></a></p>

<p align="center">
  <a href="#"><img alt="License" src="https://img.shields.io/github/license/d-Raco/android-malware-analyzer"></a>
  <a href="#"><img alt="Platform" src="https://shields.io/badge/platform-android%20|%20linux-%23989898"></a>
  <a href="#"><img alt="Last commit" src="https://shields.io/github/last-commit/d-Raco/android-malware-analyzer"></a>
  <a href="#"><img alt="Repo stars" src="https://img.shields.io/github/stars/d-Raco/android-malware-analyzer"></a>
  <a href="#"><img alt="Repo watchers" src="https://img.shields.io/github/watchers/d-Raco/android-malware-analyzer"></a>
</p>

# android-malware-analyzer

Android application to detect malware based on permissions, hashes and logs.

Documentation for this project can be found [here](https://eprints.ucm.es/id/eprint/66842/) and [here](https://github.com/d-Raco/android-malware-analyzer/blob/master/docs/Malware-Analysis-on-Android.pdf).

## Table of Contents

[android-malware-analyzer](https://github.com/d-Raco/android-malware-analyzer#android-malware-analyzer)
+ [Table of Contents](https://github.com/d-Raco/android-malware-analyzer#table-of-contents)
+ [Introduction](https://github.com/d-Raco/android-malware-analyzer#introduction)
+ [Motivation](https://github.com/d-Raco/android-malware-analyzer#motivation)
+ [Proposed Solution](https://github.com/d-Raco/android-malware-analyzer#proposed-solution)
+ [Decisions & Features](https://github.com/d-Raco/android-malware-analyzer#decisions--features)
+ [Requirements](https://github.com/d-Raco/android-malware-analyzer#requirements)
+ [File Structure](https://github.com/d-Raco/android-malware-analyzer#file-structure)
+ [Conclusions](https://github.com/d-Raco/android-malware-analyzer#conclusions)
+ [Future Work](https://github.com/d-Raco/android-malware-analyzer#future-work)
  - [Signature Analyzer](https://github.com/d-Raco/android-malware-analyzer#signature-analyzer)
  - [Log Analyzer](https://github.com/d-Raco/android-malware-analyzer#log-analyzer)
  - [Permission Analyzer](https://github.com/d-Raco/android-malware-analyzer#permission-analyzer)
+ [Resources](https://github.com/d-Raco/android-malware-analyzer#resources)

## Introduction

In the XXI century, the world has witnessed the creation, development and proliferation of mobile devices until the massive usage apparent nowadays. The portability, instantaneity and ease of use that these devices offer has encouraged the great majority of the population to have one of them at arm’s length. Thus, these devices have become a coveted target for malicious developers. This is the reason why the security of mobile devices has become a vital topic that must be addressed, since a suitable solution has yet to be found.

From this necessity arises the present work, in which we elaborate the beginning of a response that serves as a starting point to promote further development that achieves the desired objective.

With Android being the most representative Operating System among mobile devices, we are going to study the analysis of malware on Android and develop a static and dynamic antivirus based on signatures, permissions and logs, since they will prove useful when trying to detect malicious applications.

## Motivation

We live in a time in which the proliferation of mobile devices is progressively increasing, thus leading to the estimate that currently 70% of the world's population uses them. Everyone from children to seniors uses mobile devices in different ways, depending on their preferences, hobbies, or business purposes.

More than 2,500 million users choose to use devices with the Android operating system, which translates into an 85.9% share in the global mobile market. Therefore, our work has focused on Android as it is the most widely distributed mobile operating system in the world.

There is currently a long list of antivirus products that any user can use to protect and prevent attacks on their device. However, most anti-malware has been developed with Windows or Mac OS operating systems in mind, leaving Android on the sidelines. Let us point out that the apparent lack of concern for its security for Android is ironic. There are many Android apps that evade security mechanisms such as Google Play Protect's analysis before being put on the Play Store, and others that are not initially malicious, but then receive an update that changes everything. The increase of malicious applications on Android devices appearing both inside and outside the Play Store itself, together with new attack techniques aimed at mobile devices (SIM Swapping, Smishing, etc.) demonstrate the need for more work in Android security.

Therefore, the motivation for this project is to study the potential and effectiveness of certain malware analysis techniques adapted to the Android operating system. In addition to the benefit of combining these different techniques in a single application.

## Proposed Solution

An application that analyzes all installed apps using three methods:
- Extract their permissions and calculate a score based on their quantity and protection level.
- Calculate their hash and compare it to a malware signature database.
- Extract all logs in real time, send them to a server and analyze them in the cloud and send the result back to the device.

## Decisions & Features

- We decided to implement a log analysis for Android because we wanted to have a dynamic analysis to add some functionality over the two other methods of analysis. Also, since we did not find much information about it, we wanted to try and create something relatively new.
- We opt to implement a permission analyzer because it can give the user plenty of information about what the application is trying to achieve. Also, by giving a score we believe we can show visually and effectively if it is actually a benevolent application. Finally, we think that it can raise the user’s awareness regarding permission granting and encourage them to check the permissions of the applications being installed on their device.
- We chose to implement a signature analyzer since if there is a collision found with its signature, it is almost certain that it is malware, implying that this analysis method gives an almost absolute certainty to the user.
- We decided to implement a Server Settings Fragment because every time the EC2 instance is launched, a new IP is set for the instance. If we had not developed this fragment, each time we launched the instance we would have needed to write the new IP in the application code, build the APK and install it on our mobile phones. Also, this fragment allows the user to create their own server and only have to worry about adding the IP without changing any code.
- We chose to process the logs on the cloud because doing it locally would have implied that the application would probably lag or crash due to the amount of processing power needed. It also implies that the battery usage is reduced, since less power is needed. Finally, this allows the user to set up their own server for processing the logs.
- We also concluded that we would only analyze the applications installed by the user and not the stock applications since these applications are developed by relevant companies which are globally trusted. Also, since the user cannot uninstall them, it would only mean that the analyses would take more time and they would not gain anything.
- Easily set IP and port of the open server that will receive and analyze the logs.
- History of all previous analyses.

## Requirements

- Android Studio
- ADB
- Cloud Instance
- Spark Streaming

Android Studio is used to develop the Android app, and can be downloaded from [this](https://developer.android.com/studio/#downloads) web page.

ADB (Android Debug Bridge) can be downloaded from [here](https://developer.android.com/studio/releases/platform-tools). It is a command line tool that allows the user to communicate with the Android device through a console from the PC. For this project, it has been necessary to do an analysis of all the logs that Android launches, so we must prepare the device to take advantage of all its functionalities. For example, to access all the `logcat` logs, the `android.permission.READ_LOGS` permission must be granted to the analyzer app. This permission is of the special category due to its potential danger, which is why the permission to read logs should only be granted to the applications that are part of the system firmware. To get our application to benefit from this permission we must use the ADB tool. The command `adb shell pm grant com.example.androidmalwareanalyzer android.permission.READ_LOGS` used in the ADB console, allows to grant permissions to the malware analyzer. It is worth mentioning that to use this tool it is necessary to have the USB Debugging of the device activated.

A Cloud Instance is necessary to hold the server-side code to analyze the logs. Spark Streaming is used to receive and analyze those logs in real time.

## File Structure

```graphql
# Important project components
.
├── AndroidMalwareAnalyzer - # Contains the Android app code
│   ├── app
│   │   └── src
│   │       └── main - # App code
│   │           ├── AndroidManifest.xml
│   │           ├── java
│   │           │   └── com
│   │           │       └── example
│   │           │           └── androidmalwareanalyzer
│   │           │               ├── MainActivity.java
│   │           │               └── ui
│   │           │                   ├── aboutUs - # Print developer information
│   │           │                   │   └── AboutUsFragment.java
│   │           │                   ├── appsInformation - # Extract information from apps
│   │           │                   │   ├── AppDetails.java
│   │           │                   │   ├── AppPermissions.java
│   │           │                   │   ├── AppsListFragment.java
│   │           │                   │   ├── PackageInfoStruct.java
│   │           │                   │   └── PermissionFragment.java
│   │           │                   ├── home - # Home view
│   │           │                   │   └── HomeFragment.java
│   │           │                   ├── logAnalyzer - # Analyze logs
│   │           │                   │   ├── LogAnalyzerConnect.java
│   │           │                   │   ├── LogAnalyzerFragmentApplications.java
│   │           │                   │   ├── LogAnalyzerFragment.java
│   │           │                   │   ├── LogAppsAdapter.java
│   │           │                   │   └── LogResult.java
│   │           │                   ├── MalwareDB.java - # Database holding malware hashes and permissions
│   │           │                   ├── permissionAnalyzer - # Analyze permissions
│   │           │                   │   ├── AppScore.java
│   │           │                   │   ├── CustomExpandableListAdapter.java
│   │           │                   │   ├── ExpandableListDataPump.java
│   │           │                   │   ├── PermissionAnalysisProcess.java
│   │           │                   │   └── PermissionAnalyzerFragment.java
│   │           │                   ├── prevResults - # Show a history of previous analysis results
│   │           │                   │   ├── ElementsAdapter.java
│   │           │                   │   ├── LogElementsAdapter.java
│   │           │                   │   ├── LogSubElementsAdapter.java
│   │           │                   │   ├── PrevResultsAdapter.java
│   │           │                   │   ├── PrevResultsDB.java
│   │           │                   │   ├── PrevResultsFragment.java
│   │           │                   │   ├── PrevResultsRecyclerView.java
│   │           │                   │   ├── ShowLogResult.java
│   │           │                   │   └── ShowResult.java
│   │           │                   ├── serverSettings - # Set up IP and port of the server that will receive the logs
│   │           │                   │   └── ServerSettingsFragment.java
│   │           │                   └── signatureAnalyzer - # Analyze hashes
│   │           │                       ├── AppInfo.java
│   │           │                       ├── AppsManager.java
│   │           │                       ├── InstalledAppsAdapter.java
│   │           │                       └── SignatureAnalyzerFragment.java
│   │           └── res - # App visual resources
├── Android_Malware_Analyzer.apk - # Compiled Android app
├── docs
│   └── Malware-Analysis-on-Android.pdf - # Thesis
└── server - # Contains the server code
    ├── analyser.py - # Analyzes the logs
    └── server.c - # Opens a socket and receives logs
```

## Conclusions

At the end of the project and after having exposed the necessity of an anti-malware software installed on the device of every user, we believe that our application can fill some of the crucial needs that every user of a mobile phone requires.

We believe that the objectives set at the beginning of the project have been met. We have developed an application that gives the user plenty of information that can hugely help determine if an installed app is malware or at the very least if it has suspicious behavior.

Not only have we managed to develop three different types of analysis that have proven to be extremely useful, but we also have programmed a way to display general information on the installed apps. In addition, thanks to saving the previous results, the user can track the applications behavior in different periods of time, allowing the user to get an overall image of the installed apps behavior.

We also think that the application will encourage users to actively analyze their applications and that they will learn the importance of ensuring the veracity of the applications being installed based on their permissions and their behavior.

## Future Work

### Signature Analyzer

- Expand the malware dataset of digital signatures (hashes), as well as being able to store for each database row different algorithms of hashing such as SHA512, SHA256, etc applied on a single malware.
- Migrate the database to the Cloud so that the devices that have installed our application do not need to have it locally, thereby taking up less space and having higher performance.
- Create another database or modify and extend the existing one to contain the hashes of the top 100 (or more) most used/installed apps in the Google Play Store in order to compare the hashes of the apps installed on your mobile with those in the database to detect malware on your Android device.
- Create a reporting option for the user in case she/he detects a new malicious application by using other analysis methods (Log analyzer or Permission analyzer) or from another source, in order to generate its hash and include it in our database.

### Log Analyzer

- Configure the server so that the scripts can be running all the time. Right now, the only way to achieve this is by having the scripts running in a shell that is connected to the server through ssh, which is not ideal.
- In the results, show the time when each target element was accessed.
- Perform the connection to the server through SSL.
- Change the AsyncTask for an Executor or a Service, since the AsyncTask is deprecated and can be unreliable sometimes.
- Ensure that the connection is performed from an AndroidMalwareAnalyzer application to prevent malicious connections.
- Use the log history we have created on the server to allow users to query their logs and search by date or application.
- Implement machine learning for contrasting logs of specific applications between different users, which would allow the application to detect deviant behavior.
- Improve the analysis to avoid false positives when detecting target elements.
- Develop an automatic service that launches the log analysis every certain period of time (for example, every hour for 5 minutes) and briefs the user of the results obtained from the last time the application was accessed.

### Permission Analyzer

- Store in the database all the application permissions and, in each analysis, look for any changes in the permissions in order to warn the user. Finally, keep updated the database with these changes.
- Reduce the analysis time by creating new threads.
- Get all the application categories acknowledged by the Google Play Store in order to be more precise when trying to predict the volume of permissions. This can be done, for example, implementing android-market-api.
- Improve the navigability and the GUI.
- Keep updated and complete the dataset of permissions and domains with those still unknown by the application.

## Resources

For more information, the resulting research thesis can be obtained [here](https://eprints.ucm.es/id/eprint/66842/) and also inside this repo, [here](https://github.com/d-Raco/android-malware-analyzer/blob/main/docs/Malware-Analysis-on-Android.pdf).
