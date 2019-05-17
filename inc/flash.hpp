/**
 * @file flash.hpp
 * @author Alexandre Charrier
 * @version 1.0
 * @date 20 Fev 2019
 * @brief Librarie permettant d'acceder à la mémoire flash (AT45DQ321 30Mbit) via un port SPI pour le fonctionnement du Révolution (Afficheur DEL rotatif).
 *
 * La mémoire flash est configurée principalement pour contenir et acceder des images BMP 256 x 192 x 16bit.
 * Des données de configuration servant au bon fonctionnement de l'afficheur DEL y sont également sauvegardées.
 *
 * Les images sauvegardés sont préalablement traitées par l'entremise du serveur WEB du Révolution (afficheur DEL)
 * pour être directement transféré dans les LED Drivers TLC5955.
 * Elles sont sauvegardées colonnes par colonnes et elles sont lues de la même façon.
 *
 */

#ifndef FLASH_HPP_
#define FLASH_HPP_

#include "stm32f4xx_spi.h"
#include "hardware.h"
#include <stdio.h>

class Flash {
public:
	///Positions des images dans le carrousel. Ex:xx00000000000101 = Images 0 et 2 présentes.
	uint16_t positionOfPresentImages = 0;
	///Paramètre de courrant Maximum pour le TLC5955
	uint8_t redMaxCurrent = 127;
	///Paramètre de courrant Maximum pour le TLC5955
	uint8_t greenMaxCurrent = 127;
	///Paramètre de courrant Maximum pour le TLC5955
	uint8_t blueMaxCurrent = 127;
	///Paramètre de courrant Maximum pour le TLC5955
	uint8_t globalBrightness = 127;
	/**
	 * Structure d'address pour accéder la mémoire flash.
	 * Les 8 191 pages contiennent 512 bytes chacunes.
	 */
	struct address_t {
		uint16_t page = 0;
		uint16_t byte = 0;
	};
	/**
	 *
	 * @param debug : Active la sortie des messages de déverminage vers la console
	 * @return L'instance de l'objet FLash
	 */
	static Flash* getInstance(bool debug = 0);
	///Images maximums pouvant être sauvegardées dans la flash
	static constexpr uint8_t MaxImageStored = 14;
	///Nombre de caractères maximum par nom de fichier
	static constexpr uint16_t FilenameSize = 16;
	///Taille du tableau de donnés envoyé vers trois TLC5955 par port SPI
	static constexpr uint16_t SPIBufferSize = 289;
	///Taille total d'une colonne de 192 pixels
	static constexpr uint16_t ColumnPixelArraySize = 1156;
	///Taille d'un fichier d'image
	static constexpr uint32_t ImageFileSize = 295936;
	///Nombre de pages de mémoire par fichier d'image.
	static constexpr uint16_t PagesPerImage = 578;
	///Adresse de la première image du carrousel
	static constexpr uint8_t FirstImagePageAddress = 99;
	///Taille des pages de mémoire flash en Byte
	static constexpr uint16_t PageSize = 512;
	///Résolution de l'axe de X maximum
	static constexpr uint16_t MaxColumnCount = 256;
	/**
	 * Destructeur de l'objet
	 */
	~Flash();
	/**
	 * Initialise le port SPI5 du STM32F411RE
	 */
	void init();
	/**
	 * Methode d'envoie et de réception pour le port SPI
	 * @param data : Donné en byte à transférer dans le port SPI
	 * @return byte lu par le port SPI
	 */
	uint8_t inline spiTransfer(uint8_t data);
	/**
	 * Active et désactive le Chip Select du port SPI
	 * @param state : 1 = active le Chip Select,  0 = désactive le Chip Select
	 */
	void setCS(bool state);
	/**
	 * Intéroge la flash sur son état d'activité
	 * @return busy state. 0 = Flash est prêt pour une nouvelle opération
	 */
	bool isBusy();
	/**
	 * Lie le regsitre d'état de la mémoire flash
	 * @return Le registre d'état de la flash
	 */
	uint16_t readStatusRegister();
	/**
	 * Lie le regsitre de configuration de la mémoire flash
	 * @return Le registre de configuration de la flash
	 */
	uint8_t readConfigurationRegister();
	/**
	 * Lie le registre de configuration et l'envoie en format texte dans la console
	 */
	void readStatusRegisterToString();
	/**
	 * Écrit un byte dans la flash à une adresse donnée
	 * @param add : Adresse de la flash
	 * @param byte : Byte à écrire
	 */
	void writeByte(const address_t *add, uint8_t byte);
	/**
	 * Écrit un tableau de byte dans la flash à une adresse donnée
	 * @param add : Adresse de la flash
	 * @param byte : Tableau de uint8_t à transférer
	 * @param nByte : nombre de bytes à tranférer. À noter que les pages contiennent 512 Bytes.
	 * si l'écriture dépasse le byte 511, l'écriture se continue au début de la même page au byte 0.
	 * @param offsetByte : Décalage de lecture du tableau à transférer.
	 * Ex:  235 signifirait que la lecture des donnés à écrire dans la flash débuterait à la 235e case du tableau (byte[235]).
	 */
	void writeByte(const address_t *add, uint8_t *byte, uint16_t nByte,
			uint16_t offsetByte = 0);
	/**
	 * Écrit un tableau de byte dans la flash à une adresse donnée
	 * @param add : Adresse de la flash de départ
	 * @param byte : Tableau de uint8_t à transférer
	 * @param nByte : nombre de bytes à tranférer. À noter que les pages contiennent 512 Bytes.
	 * si l'écriture dépasse le byte 511, l'écriture se continuera au début de la même page au byte 0.
	 */
	void writeByte(const address_t *add, const char *byte, uint16_t nByte);
	/**
	 * Écrit les deux bytes d'un uint16_t dans la flash
	 * @param add : Adresse de la flash de départ
	 * @param byte : uint16_t à écrire (2 bytes)
	 */
	void writeByteU16(const address_t *add, uint16_t byte);
	/**
	 * Écrit les quatre bytes d'un uint32_t dans la flash
	 * @param add : Adresse de la flash de départ
	 * @param byte : uint32_t à écrire (4 bytes)
	 */
	void writeByteU32(const address_t *add, uint32_t byte);
	/**
	 * Lie un byte dans la flash à une adresse donnée
	 * @param add : Adresse de la flash
	 * @return Le byte lu à l'adresse
	 */
	uint8_t readByte(const address_t *add);
	/**
	 * Lie une série de bytes dans la flash à partir d'une adresse donnée
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranférée.
	 * @param nBytes : nombre de bytes à transférer. À noter que les pages contiennent 512 Bytes.
	 * Si la lecture dépasse le byte 511, la lecture se continuera au début de la même page au byte 0.
	 */
	void readByte(const address_t *add, uint8_t *buffer, uint16_t nBytes);
	/**
	 * Lie une série de bytes dans la flash à partir d'une adresse donnée
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranférée.
	 * @param nBytes : nombre de bytes à transférer. À noter que les pages contiennent 512 Bytes.
	 * Si la lecture dépasse le byte 511, la lecture se continuera au début de la même page au byte 0.
	 */
	void readByte(const address_t *add, char *buffer, uint16_t nBytes);
	/**
	 * Lecture en continue d'une page à l'autre.
	 * La lecture se fait en continue et la flash incrémente
	 * automatiquement l'adresse pour lire les pages précédantes.
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranférée.
	 * @param nBytes : nombre de bytes à transférer. La lecture se fait en continue et la flash incrémente
	 * automatiquement l'adresse pour lire les pages précédantes.
	 */
	void readPageArray(const address_t *add, uint8_t *buffer, uint32_t nByte);
	/**
	 * Efface le contenue de la page à l'adresse spécifiée.
	 * @param add : Adresse de la flash
	 */
	void formatPage(const address_t *add);
	/**
	 * Efface la mémoire entière de la flash.
	 */
	void formatChip();
	/**
	 * Désactive la protection d'écriture sélective de la flash
	 */
	void disableSectorProtection();
	/**
	 * Lie l'indetification du manifacturier dans la flash
	 * @param buffer : Tableau de grandeur minimum de 5 dans lequelle le Device id est retourné.
	 */
	void getDeviceID(uint8_t *buffer);
	/**
	 * Méthode d'initialisation des formats de page en 512 Bytes par page de mémoire.
	 */
	void setPageSizeBinary();
	/**
	 * Retrouve les données de controle du TLC5955 enregistrées dans la Flash. Ces données sont emagasinées
	 * dans les membres de l'objet : redMaxCurrent, greenMaxCurrent, blueMaxCurrent et gloalBrightness.
	 */
	void readControlRegister();
	/**
	 * Sauvegarde les données de controle du TLC5955 enregistrées dans la Flash. Ces données proviennent
	 * dess membres de l'objet : redMaxCurrent, greenMaxCurrent, blueMaxCurrent et gloalBrightness.
	 */
	void writeControlRegister();
	/**
	 * Intéroge la flash pour savoir les images présentes dans le caroussel
	 * @return uint16_t contenant les positions d'images présentes dans le carrousel.
	 * Chaque bit représente une image (Max 14 images)
	 * Ex: xx00010000000011 ----> Images 0, 1 et 10 présentes dans le carrousel.
	 */
	uint16_t getPositionOfPresentImagesInCarrousel();
	/**
	 * Sauvegarde le positionnement des images présentes dans le carrousel.
	 * Chaque bit représente une image (Max 14 images)
	 * Ex: xx00010000000011 ----> Images 0, 1 et 10 présentes dans le carrousel.
	 */
	void savePositionOfPresentImagesInCarrousel();
	/**
	 * Inscrit une image à afficher dans le carrousel
	 * @param imageNo : Le numéro de l'image à inscrire du carrousel.
	 */
	void setImageInCarrousel(uint8_t imageNo);
	/**
	 * Retire une image à afficher dans le carrousel
	 * @param imageNo : Le numéro de l'image à retirer du carrousel.
	 */
	void resetImageInCarrousel(uint8_t imageNo);
	/**
	 * Vide le carrousel
	 */
	void formatCarrousel();
	/**
	 * Retourne le nombre d'image inscrites dans le carrousel
	 * @return Le nombre d'images disponible dans le carrousel.
	 */
	uint8_t getNumberOfImagesInCarrousel();
	/**
	 * Retourne le prochain emplacement libre pour sauvegader une image.
	 * Si le carrousel est plein, l'image no 0 sera écrasée.
	 * @return uint8_t avec la valeur décimal de l'emplacement vide
	 */
	uint8_t getNextFreeImageSlot();
	/**
	 * Retrouve le nom de fichier d'une image
	 * @param imageNo : Le numéro d'image.
	 * @param destination : Tableau dans lequel le nom sera transféré. Le tableau doit au minimum de FilenameSize.
	 */
	void getFilename(uint8_t imageNo, char *destination);
	/**
	 * Écrit le nom du fichier d'un image
	 * @param imageNo : Le numéro de l'image.
	 * @param fileName : Tableau du nom du fichier. Si le tableau est plus grad que FilenameSize
	 */
	void setFilename(uint8_t imageNo, uint8_t *fileName);
	/**
	 * Remet à zéro l'espace alloué pour le nom de fichier d'une image
	 * @param imageNo
	 */
	void resetFilename(uint8_t imageNo);
	/**
	 * Lie une colonne d'affichage emagasiné dans la flash par l'entremise de la DMA
	 * @param imageNo : Numéro de l'image
	 * @param columnNo : Numéro de la colonne
	 * @param spiBuffer : Tableau dans lequel les données sont transférés
	 * @return
	 */
	bool getPixelColumnDMA(uint8_t imageNo, uint8_t columnNo,
			uint8_t* spiBuffer);
	bool getPixelColumn(uint8_t imageNo, uint8_t columnNo, uint8_t* spiBuffer);
	bool savePixelColumn(uint8_t imageNo, uint8_t columnNo, uint8_t* source);
	void getPixelColumnToString(uint8_t imageNo, uint8_t columnNo);
	void setDebug(bool debug);
	uint8_t countSetBits(uint32_t n);

private:
	Flash();
	Flash(bool debugEnable);

	static Flash* instance;

	/**
	 * Flash read/write and SPI command
	 */

	static constexpr uint16_t BusyFlag = 0x8080;
	static constexpr uint16_t ReadWriteErrorFlag = 0x0020;

	static constexpr uint8_t DummyByte = 0x00;

	static constexpr uint8_t MainMemmoryPageRead = 0xD2;
	static constexpr uint8_t ContinuousPageRead = 0x1B;
	static constexpr uint8_t WrtitePagesThroughBuf1BIE = 0x82;
	static constexpr uint8_t WrtitePagesThroughBuf2BIE = 0x85;
	static constexpr uint8_t WrtiteBytesThroughBuf1NoBIE = 0x02;
	static constexpr uint8_t PageErase = 0x81;
	static constexpr uint8_t DeviceID = 0x9F;
	static constexpr uint8_t StatusRegisterRead = 0xD7;
	static constexpr uint8_t ConfigurationRegisterRead = 0x3F;

	static constexpr uint8_t BinaryPageSize[4] = { 0x3D, 0x2A, 0x80, 0xA6 };
	static constexpr uint8_t ChipErase[4] = { 0xC7, 0x94, 0x80, 0x9A };
	static constexpr uint8_t DisableSectorProtect[4] =
			{ 0x3D, 0x2A, 0x7F, 0x9A };

	/**
	 * Static configuration addresses
	 */

	/*
	 * Page 9 is reserved for BMP file names: 14 * 16 char = 240 bytes ex:123456789012.bmp
	 */

	const address_t FilenamePage = { 9, 0 }; //14 images * 16 char = 226 bytes
	const address_t GlobalBrightnessSettingAddress = { 10, 0 };		//2 bytes
	const address_t RedMaxCurrentSettingAddress = { 10, 2 };		//2 bytes
	const address_t GreenMaxCurrentSettingAddress = { 10, 4 };		//2 bytes
	const address_t BlueMaxCurrentSettingAddress = { 10, 6 };		//2 Bytes
	const address_t PositionOfPresentImagesInCarrouselAddress = { 10, 8 };//2 bytes

	bool debug = false;

};

#endif /* FLASH_HPP_ */

