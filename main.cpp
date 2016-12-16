
#include <windows.h>

#include <stdexcept>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <cassert>

#include <foobar/Cmdline.hxx>
#include <pkcs11.hxx>

using std::cout;
using std::endl;

int main(int argc, const char* argv[])
{

	try
	{
		auto cmdl = foobar::Cmdline::Parse("m:|module:,p:|pin:", argv, argc);

		if (!cmdl->opt["m"].Specified())
			throw std::runtime_error("pk11 module is not specified");
		if (!cmdl->opt["p"].Specified())
			throw std::runtime_error("card pin is not specified");

		std::vector<pkcs11::Slot> lst = pkcs11::load_modulus(cmdl->opt["m"].Str())->ListSlots();
		if (!lst.size())
			throw std::runtime_error("there are no tokens");

		pkcs11::Slot slt = lst[0];
		cout << "using slot " << slt->Id() << endl;

		/*std::vector<pkcs11::Mechanism> mcs = slt->ListMechanisms(CKF_DECRYPT);
		for (auto i = mcs.begin(); i!= mcs.end(); ++i )
		{
			cout << (*i)->ToString() << endl;
		}*/

		pkcs11::Session ssn = slt->Open();
		ssn->Login(cmdl->opt["p"].Str());
		std::vector<pkcs11::KeyPair> keys = ssn->ListKeys();
		//std::vector<pkcs11::Cert> certs = ssn->ListCerts();
		//cout << certs[0]->Body().size() << endl;

		/*for ( auto i = certs.begin(); i != certs.end(); ++i )
		{
			cout << (*i)->Body().size() << endl;
		}*/
	
        auto mcs = slt->ListMechanisms(CKF_SIGN|CKF_VERIFY);
		for (auto i = mcs.begin(); i!= mcs.end(); ++i )
		{
			cout << (*i)->ToString() << endl;
		}


		//auto i = keys.begin();
		for (auto i = keys.begin(); i != keys.end(); ++i)
		{
            std::vector<uint8_t> data((*i)->Bits()/8-40);
			//std::generate(data.begin(), data.end(), rand);
			for ( int j = 0; j < data.size(); ++j ) data[j] = j;
            auto cert = (*i)->GetCert();
            /*std::vector<uint8_t> tp = cert->Sha1();
            for ( int j = 0; j < tp.size(); ++j )
                printf("%02x ",tp[j]);
            printf("\n");
            */
            cout << (*i)->Id() << " " << (*i)->Bits() << " bits, ";
            printf("signing..\n");
            std::vector<uint8_t> E = (*i)->Sign(data,CKM_RSA_PKCS);
            std::vector<uint8_t> Q = (*i)->Encrypt(E,CKM_RSA_X_509);
            for ( int j = 0; j < Q.size(); ++j )
                printf("%02x ",Q[j]);
            //std::vector<uint8_t> E = (*i)->Decrypt(data,CKM_RSA_X_509);
            //
            //cout << ( data == Q ? "succeeded": "failed") << endl;
            //printf("verifying..\n");
            //bool ok = (*i)->Verify(data, E, CKM_RSA_PKCS);
            //cout << "sing/verify test: " << (ok ? "succeeded" : "failed") << endl;

			/*
			//std::vector<uint8_t> data(rand() % 10000 + 1);
			std::vector<uint8_t> data(256-14);
			//std::generate(data.begin(), data.end(), rand);
			for ( int j = 0; j < data.size(); ++j ) data[j] = j;
			//std::fill(data.begin(), data.end(), 0x03);
			cout << data.size() << endl;
			std::vector<uint8_t> encrypted  = (*i)->Encrypt(data,CKM_RSA_PKCS);
			std::vector<uint8_t> signature1 = (*i)->Sign(encrypted,CKM_RSA_X_509);
			//std::vector<uint8_t> signature2 = (*i)->Encrypt(signature1,CKM_RSA_X_509);
			for ( int i = 0; i < signature1.size(); ++i )
				printf("%02x ",signature1[i]);
			printf("\n");
			std::vector<uint8_t> signature3 = (*i)->Sign(signature1,CKM_RSA_X_509);
			//bool ok = (*i)->Verify(data, signature1, CKM_SHA1_RSA_PKCS);
			//cout << (*i)->Id() << " " << (*i)->Bits() << " bits, ";
			//cout << "sing/verify test: " << (ok ? "succeeded" : "failed") << endl;
			*/
		}

		/*for (auto i = keys.begin()+2; i != keys.end(); ++i)
		{
			//for (auto m = mcs.begin(); m!= mcs.end(); ++m )
			{
				//cout << (*m)->ToString() << endl;
				cout << (*i)->Bits() << endl;
				//std::vector<uint8_t> data((*m)->Id() == CKM_RSA_X_509?(*i)->Bits()/8-1:(*i)->Bits()/8-42);
				std::vector<uint8_t> data((*i)->Bits()/8);
				cout << data.size() << endl;
				std::generate(data.begin(), data.end(), rand);
				data[0] = 0;
				//std::vector<uint8_t> encrypted = (*i)->Encrypt(data,(*m)->Id());
				//std::vector<uint8_t> decrypted = (*i)->Decrypt(encrypted,(*m)->Id());
				std::vector<uint8_t> encrypted = (*i)->Encrypt(data,CKM_RSA_X_509);
				std::vector<uint8_t> decrypted = (*i)->Decrypt(encrypted,CKM_RSA_X_509);
				//cout << (*i)->Id() << " " << (*m)->ToString()<< " " << (*i)->Bits() << " bits, ";
				cout << "encrypt/decrypt test: " << ((data == decrypted)?"succeeded":"failed") << endl;
			}
		}*/

		return 0;
	}
	catch (std::exception& e)
	{
		cout << "error: " << e.what() << endl;
		return -1;
	}
}
